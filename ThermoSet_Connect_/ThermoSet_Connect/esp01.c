#include <string.h>
#include <stdio.h>
#include "uart.h"
#include "delay.h"
#include "lcd_defines.h"
#include "esp01.h"

extern char buff[300];
extern int i;

/* Check ESP01 response using AT command */
int esp01_connectAP_AT()
{
        int timeout = 0;

        string_uart("AT\r\n");

        i = 0;
        memset(buff,'\0',300);

        while((i < 3) && (timeout < 20))
        {
                delay_ms(100);
                timeout++;
        }

        if(timeout >= 20)
        {
                return -1;
        }

        delay_ms(500);

        buff[i] = '\0';

        if(strstr(buff,"OK"))
        {
                return 1;
        }
        else
        {
                return 0;
        }
}

/* Disable echo */
int esp01_connectAP_ATE0()
{
        int timeout = 0;

        string_uart("ATE0\r\n");

        i = 0;
        memset(buff,'\0',300);

        while((i < 4) && (timeout < 20))
        {
                delay_ms(100);
                timeout++;
        }

        if(timeout >= 20)
        {
                return -1;
        }

        delay_ms(500);

        buff[i] = '\0';

        if(strstr(buff,"OK"))
        {
                return 1;
        }
        else
        {
                return 0;
        }
}

/* Configure single TCP connection mode */
int esp01_connectAP_TCP_MODE()
{
        int timeout = 0;

        string_uart("AT+CIPMUX=0\r\n");

        i = 0;
        memset(buff,'\0',300);

        while((i < 4) && (timeout < 20))
        {
                delay_ms(100);
                timeout++;
        }

        if(timeout >= 20)
        {
                return -1;
        }

        delay_ms(500);

        buff[i] = '\0';

        if(strstr(buff,"OK"))
        {
                return 1;
        }
        else
        {
                return 0;
        }
}

/* Disconnect previous WiFi connection */
int esp01_connectAP_QUIT_AP()
{
        int timeout = 0;

        delay_ms(1000);

        string_uart("AT+CWQAP\r\n");

        i = 0;
        memset(buff,'\0',300);

        while((i < 4) && (timeout < 20))
        {
                delay_ms(100);
                timeout++;
        }

        if(timeout >= 20)
        {
                return -1;
        }

        delay_ms(1500);

        buff[i] = '\0';

        if(strstr(buff,"OK"))
        {
                return 1;
        }
        else
        {
                return 0;
        }
}

/* Connect ESP01 to the configured WiFi access point */
int esp01_connectAP_JOIN_AP()
{
        int timeout = 0;

        string_uart("AT+CWJAP=\"");
        string_uart(WIFI_SSID);
        string_uart("\",\"");
        string_uart(WIFI_PASSWORD);
        string_uart("\"\r\n");

        i = 0;
        memset(buff,'\0',300);

        while((i < 4) && (timeout < 100))
        {
                delay_ms(100);
                timeout++;
        }

        if(timeout >= 100)
        {
                return -1;
        }

        delay_ms(2500);

        buff[i] = '\0';

        if(strstr(buff,"WIFI CONNECTED") || strstr(buff,"OK"))
        {
                return 1;
        }
        else
        {
                return 0;
        }
}

/* Upload data to ThingSpeak cloud (main data channel) */
int esp01_sendToThingspeak(int field,int num)
{
        char req[80];
        int len;
        int timeout = 0;

        string_uart("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");

        i = 0;
        memset(buff,'\0',300);

        while((i < 5) && (timeout < 30))
        {
                delay_ms(100);
                timeout++;
        }

        if(timeout >= 30)
        {
                return -1;
        }

        delay_ms(1500);

        buff[i] = '\0';

        if(strstr(buff,"CONNECT") || strstr(buff,"ALREADY CONNECTED"))
        {
                /* Build the HTTP GET request for the write channel */
                sprintf(req,"GET /update?api_key=%s&field%d=%d\r\n\r\n",
                        TS_WRITE_API_KEY, field, num);

                len = strlen(req);

                string_uart("AT+CIPSEND=");
                int_uart(len);
                string_uart("\r\n");

                delay_ms(500);

                i = 0;
                memset(buff,'\0',300);

                string_uart(req);

                delay_ms(3000);

                buff[i] = '\0';

                if(strstr(buff,"SEND OK"))
                {
                        return 1;
                }
                else
                {
                        return 0;
                }
        }
        else
        {
                return 0;
        }
}

/* Read the latest value of a field from a ThingSpeak channel
   (used to fetch the remotely-entered set point from the
   dedicated set-point entry channel)                          */
int esp01_readThingspeakField(unsigned int channel_id,int field,char *read_key)
{
        char req[100];
        int len;
        int timeout = 0;
        char *p;
        int val;

        string_uart("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");

        i = 0;
        memset(buff,'\0',300);

        while((i < 5) && (timeout < 30))
        {
                delay_ms(100);
                timeout++;
        }

        if(timeout >= 30)
        {
                return -9999;
        }

        delay_ms(1500);

        buff[i] = '\0';

        if(!(strstr(buff,"CONNECT") || strstr(buff,"ALREADY CONNECTED")))
        {
                return -9999;
        }

        /* Build the HTTP GET request; last.txt returns only the raw value */
        sprintf(req,"GET /channels/%u/fields/%d/last.txt?api_key=%s\r\n\r\n",
                channel_id, field, read_key);

        len = strlen(req);

        string_uart("AT+CIPSEND=");
        int_uart(len);
        string_uart("\r\n");

        delay_ms(500);

        i = 0;
        memset(buff,'\0',300);

        string_uart(req);

        delay_ms(3000);

        buff[i] = '\0';

        /* The reply looks like:  ...SEND OK  +IPD,2:40CLOSED
           so the buffer ENDS with the word CLOSED, not with digits.
           The value is the text right after the ':' of "+IPD,<len>:".   */
        p = strstr(buff,"+IPD,");

        if(p == 0)
        {
                return -9999;
        }

        p = strchr(p,':');

        if(p == 0)
        {
                return -9999;
        }

        p++;

        while((*p == ' ') || (*p == '\r') || (*p == '\n'))
        {
                p++;
        }

        if(*p == '-')
        {
                /* empty channel answers "-1" */
                return -1;
        }

        if((*p < '0') || (*p > '9'))
        {
                return -9999;
        }

        val = 0;

        while((*p >= '0') && (*p <= '9'))
        {
                val = val * 10 + (*p - '0');
                p++;
        }

        return val;
}
