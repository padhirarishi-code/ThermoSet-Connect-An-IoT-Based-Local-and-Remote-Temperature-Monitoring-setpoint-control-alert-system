#include <lpc21xx.h>
#include "delay.h"
#include "lcd_defines.h"
#include "esp01.h"
#include "uart.h"
#include "eeprom.h"

/* External variables */
extern char buff[300];
extern int i;

/* EEPROM locations */
#define SETPOINT_ADDR 0x77   /* set point in use                         */
#define CLOUD_SP_ADDR 0x78   /* last value seen on the cloud set-point channel */

/* ThingSpeak (free plan) accepts only ONE update per channel every 15 s.
   A faster update is silently thrown away, so wait 16 s between uploads. */
#define TS_MIN_GAP_SEC 16

static int          tx_started = 0;
static unsigned int last_tx    = 0;

/* Seconds since midnight from the on-chip RTC */
static unsigned int rtc_secs(void)
{
        return ((unsigned int)HOUR*3600u)+((unsigned int)MIN*60u)+(unsigned int)SEC;
}

/* Wait (max ~20 s) until 16 s have passed since the previous upload */
static void ts_rate_limit(void)
{
        unsigned int waited=0;

        if(!tx_started)
        {
                return;
        }

        while(waited<200)
        {
                if(((rtc_secs()+86400u-last_tx)%86400u)>=TS_MIN_GAP_SEC)
                {
                        break;
                }

                delay_ms(100);
                waited++;
        }
}

/* Function to initialize ESP01 module: AT handshake + join WiFi */
void init_esp01()
{
        int cnt=0,ret,ncnt=0,tot=0;

        /* Check AT communication */
        while(1)
        {
                ret=esp01_connectAP_AT();

                if(ret==1)
                {
                        break;
                }
                else if(ret==0)
                {
                        cnt++;
                        tot++;
                }
                else
                {
                        ncnt++;
                        tot++;
                }

                if(tot>=5)
                {
                        cmd_lcd(0x01);
                        string_lcd("ESP01 not");
                        cmd_lcd(0xc0);
                        string_lcd("responding");
                        delay_ms(3000);
                        cmd_lcd(0x01);
                        return;
                }

                delay_ms(1000);
        }

        /* Disable echo */
        tot=0;
        while(1)
        {
                ret=esp01_connectAP_ATE0();

                if(ret==1)
                {
                        break;
                }

                tot++;

                if(tot>=5)
                {
                        break; /* not critical, continue anyway */
                }

                delay_ms(1000);
        }

        /* Configure single TCP connection mode */
        tot=0;
        while(1)
        {
                ret=esp01_connectAP_TCP_MODE();

                if(ret==1)
                {
                        break;
                }

                tot++;

                if(tot>=5)
                {
                        cmd_lcd(0x01);
                        string_lcd("ESP01 config");
                        cmd_lcd(0xc0);
                        string_lcd("failed");
                        delay_ms(3000);
                        cmd_lcd(0x01);
                        return;
                }

                delay_ms(1000);
        }

        /* Disconnect any previous WiFi association */
        esp01_connectAP_QUIT_AP();

        /* Join the configured WiFi network */
        tot=0;
        while(1)
        {
                ret=esp01_connectAP_JOIN_AP();

                if(ret==1)
                {
                        cmd_lcd(0x01);
                        string_lcd("WiFi connected");
                        delay_ms(1000);
                        cmd_lcd(0x01);
                        return;
                }

                tot++;

                if(tot>=5)
                {
                        cmd_lcd(0x01);
                        string_lcd("WiFi connect");
                        cmd_lcd(0xc0);
                        string_lcd("failed");
                        delay_ms(3000);
                        cmd_lcd(0x01);
                        return;
                }

                delay_ms(2000);
        }
}

/* Function to upload data to ThingSpeak (main data channel),
   with LCD status messages and a short retry loop.            */
void update_data(int field,int num)
{
        int tot=0,ret;

        cmd_lcd(0x01);

        if(field==1)
        {
                string_lcd("Uploading...");
                cmd_lcd(0xc0);
                string_lcd("Temperature");
        }
        else if(field==2)
        {
                string_lcd("Uploading...");
                cmd_lcd(0xc0);
                string_lcd("Temp Alert!");
        }
        else if(field==3)
        {
                string_lcd("Uploading...");
                cmd_lcd(0xc0);
                string_lcd("Set Point");
        }

        while(1)
        {
                ts_rate_limit();

                ret=esp01_sendToThingspeak(field,num);

                last_tx=rtc_secs();
                tx_started=1;

                if(ret==1)
                {
                        break;
                }

                tot++;

                if(tot>=3)
                {
                        cmd_lcd(0x01);
                        string_lcd("Cloud upload");
                        cmd_lcd(0xc0);
                        string_lcd("failed");
                        delay_ms(2000);
                        break;
                }

                delay_ms(1000);
        }

        cmd_lcd(0x01);
}

/* Poll the dedicated set-point entry channel on ThingSpeak.
   If the value read differs from the current set point, adopt
   it and store it in EEPROM so it survives a power loss.

   Returns: 1 = set point updated, 0 = unchanged, -1 = read failed. */
int sync_cloud_setpoint(unsigned int *set_point)
{
        int cloud_val;
        int last_cloud;

        cloud_val = esp01_readThingspeakField(SP_CHANNEL_ID, SP_FIELD, SP_READ_API_KEY);

        if(cloud_val == -9999)
        {
                return -1;
        }

        if((cloud_val < 0) || (cloud_val > 150))
        {
                /* Out-of-range / garbage reading, ignore it */
                return -1;
        }

        /* Last value seen on the cloud channel (kept in EEPROM) */
        last_cloud = (int)((unsigned char)byte_read(SA, CLOUD_SP_ADDR));

        if((unsigned int)cloud_val == *set_point)
        {
                /* already in sync - just remember what the cloud holds */
                if(last_cloud != cloud_val)
                {
                        byte_write(SA, CLOUD_SP_ADDR, (char)cloud_val);
                }

                return 0;
        }

        /* Same old cloud value as last time: the local (keypad) value is
           newer, so keep it. Without this an OLD cloud value overwrote a
           set point that had just been changed on the keypad.            */
        if(cloud_val == last_cloud)
        {
                return 0;
        }

        /* A NEW value was entered on the cloud: adopt it and save it */
        *set_point = (unsigned int)cloud_val;

        byte_write(SA, SETPOINT_ADDR, (char)(*set_point));
        byte_write(SA, CLOUD_SP_ADDR, (char)cloud_val);

        cmd_lcd(0x01);
        string_lcd("SetPoint updated");
        cmd_lcd(0xc0);
        string_lcd("from cloud");
        delay_ms(1500);
        cmd_lcd(0x01);

        return 1;
}
