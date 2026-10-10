/* ============================================================
   ThermoSet Connect
   An IoT-Based Local and Remote Temperature Monitoring,
   Set-Point Control, and Alert System

   MCU        : LPC2148
   Sensor     : LM35 (analog temperature)
   Display    : 16x2 LCD
   Keypad     : 4x4 matrix (local set point entry)
   Storage    : AT24C256 EEPROM (set point persistence)
   Connectivity: ESP01 WiFi module -> ThingSpeak cloud
   ============================================================ */

#include <lpc21xx.h>
#include "adc.h"
#include "lcd_defines.h"
#include "delay.h"
#include "rtc.h"
#include "cust_lcd.h"
#include "interrupt.h"
#include "keypad_defines.h"
#include "menu.h"
#include "i2c.h"
#include "eeprom.h"
#include "uart.h"
#include "esp01.h"

/* Buzzer pin definition (local overheat alert) */
#define TEMP_BUZZER 30

/* Upload / poll intervals, in minutes */
#define TEMP_UPLOAD_INTERVAL   3   /* how often the temperature is sent to the cloud   */
#define SP_POLL_INTERVAL       2   /* how often the cloud is checked for a new set point */

/* Buzzer alert pattern when temperature exceeds the set point */
#define BUZZER_CYCLES 3
#define BUZZER_DELAY  200

/* EEPROM address used to store the set point */
#define SETPOINT_ADDR 0x77

/* Default set point used only the very first time the system runs
   (i.e. when EEPROM has never been written / holds an invalid value) */
#define DEFAULT_SETPOINT 32

/* Global status variables */
static int last_minute = -1;
static int sp_minute_backup = -1;
static int over_temp_prev = 0;

int adc_value;
volatile int interrupt_flag = 0;   /* volatile: changed inside the ISR */
int minute_backup = 0;

unsigned int set_point;
float temperature;   /* LM35 output in VOLTS (10 mV per degC) */
int temp_c;          /* temperature in degC = volts x 100     */

/* Blink the buzzer a fixed number of cycles to warn nearby personnel */
static void buzzer_alert(void)
{
        int c;

        for(c = 0; c < BUZZER_CYCLES; c++)
        {
                IOSET0 = (1 << TEMP_BUZZER);
                delay_ms(BUZZER_DELAY);
                IOCLR0 = (1 << TEMP_BUZZER);
                delay_ms(BUZZER_DELAY);
        }
}

/* Welcome screen shown once at power-up.
   "WELCOME TO THERMOSET CONNECT" scrolls from right to left on line 1,
   then a steady "THERMOSET / CONNECT" screen is held for 2 seconds.
   Change WELCOME_SCROLL_MS to make the scroll faster / slower.        */
#define WELCOME_SCROLL_MS 120

static void welcome_msg(void)
{
        char msg[] = "WELCOME TO THERMOSET CONNECT";
        int len = 0;
        int step, col, idx;

        while(msg[len] != '\0')
        {
                len++;
        }

        cmd_lcd(0x01);
        delay_ms(5);

        /* line 2 stays fixed while line 1 scrolls (exactly 16 characters) */
        cmd_lcd(0xC0);
        string_lcd("IoT Temp Monitor");

        /* Slide a 16-character window over the message: it enters from the
           right edge, crosses the display and leaves through the left edge */
        for(step = 0; step <= (len + 16); step++)
        {
                cmd_lcd(0x80);

                for(col = 0; col < 16; col++)
                {
                        idx = step + col - 16;

                        if((idx >= 0) && (idx < len))
                        {
                                char_lcd(msg[idx]);
                        }
                        else
                        {
                                char_lcd(' ');
                        }
                }

                delay_ms(WELCOME_SCROLL_MS);
        }

        /* steady final screen */
        cmd_lcd(0x01);
        delay_ms(5);
        cmd_lcd(0x80);
        string_lcd("   THERMOSET    ");
        cmd_lcd(0xC0);
        string_lcd("    CONNECT     ");
        delay_ms(2000);

        cmd_lcd(0x01);
        delay_ms(5);
}

/* Main function starts here */
int main()
{
        /* RTC variables */
        int sec;
        int min;
        int hour;
        int date;
        int month;
        int year;
        int week;

        int stored_sp;

        /* Configure buzzer pin as GPIO output */
        PINSEL1 &= ~(3 << (2 * (TEMP_BUZZER - 16)));
        IODIR0  |= (1 << TEMP_BUZZER);

        init_interrupt();

        /* Initialize all peripherals */
        init_lcd();
        init_uart();
        init_adc();
        init_rtc();
        cust_lcd();
        init_i2c();
        init_keypad();

        /* Welcome message with scrolling text */
        welcome_msg();

        /* Set initial RTC values.
           (These variables were never given a value before, so random
           numbers were being written into the RTC. Change the date/time
           later from the menu -> 1.EDIT)                              */
        sec = 0;   min = 0;     hour = 12;
        date = 21; month = 9;   year = 2026;
        week = 1;

        set_time_info(&sec, &min, &hour);
        set_date_info(&date, &month, &year);
        set_week(&week);

        last_minute    = min;
        sp_minute_backup = min;

        /* ---- Read the stored set point back from EEPROM ----
           This is what makes the set point survive a power cycle.
           On a brand-new / blank EEPROM the read comes back invalid
           (0 or out of the 0-150 range), so a sane default is used
           and written back.                                        */
        stored_sp = (int)((unsigned char)byte_read(SA, SETPOINT_ADDR));

        if((stored_sp <= 0) || (stored_sp > 150))
        {
                set_point = DEFAULT_SETPOINT;
                byte_write(SA, SETPOINT_ADDR, (char)set_point);
        }
        else
        {
                set_point = (unsigned int)stored_sp;
        }

        cmd_lcd(0x01);

        /* Initial ADC reading */
        read_adc(1, &adc_value, &temperature);
        cmd_lcd(0x01);
        delay_ms(5);

        /* Connecting to the ESP01 */
        string_lcd("Connecting");
        delay_ms(500);
        cmd_lcd(0xC0);
        string_lcd("To ESP01");
        delay_ms(500);

        /* Initialize ESP01 + join WiFi */
        init_esp01();
        cmd_lcd(0x01);
        delay_ms(5);

        /* Main loop begins here */
        while(1)
        {
                /* Read + display RTC info */
                get_info(&sec, &min, &hour, &date, &month, &year, &week);
                display_info(sec, min, hour, date, month, year, week);

                /* Read + display temperature */
                read_adc(1, &adc_value, &temperature);
                temp_display(temperature);

                /* Volts -> degC. The set point and the cloud values are all
                   in degC, so every comparison/upload must use temp_c.     */
                temp_c = (int)(temperature * 100);

                /* ---- Periodic temperature upload ---- */
                if((((min - last_minute + 60) % 60) >= (unsigned int)TEMP_UPLOAD_INTERVAL))
                {
                        last_minute = min;
                        cmd_lcd(0x01);
                        delay_ms(5);
                        update_data(1, temp_c);
                }

                /* ---- Periodic check for a remotely-entered set point ----
                   Reads the dedicated set-point channel on ThingSpeak; if
                   the value differs from the one already stored, it is
                   adopted and saved to EEPROM. This is checked at a fixed
                   interval rather than every loop iteration, to minimize
                   unnecessary cloud traffic.                              */
                if((((min - sp_minute_backup + 60) % 60) >= (unsigned int)SP_POLL_INTERVAL))
                {
                        sp_minute_backup = min;
                        sync_cloud_setpoint(&set_point);
                }

                /* ---- Local set-point edit (switch -> interrupt -> keypad menu) ---- */
                if(interrupt_flag == 1)
                {
                        /* let the switch bounce settle, discard extra edges */
                        delay_ms(50);
                        EXTINT = (1 << 0);
                        interrupt_flag = 0;
                        minute_backup = min;

                        menu(&set_point, &sec, &min, &hour, &date, &month, &year, &week);

                        EXTINT = (1 << 0);
                        interrupt_flag = 0;

                        cmd_lcd(0x01);
                        delay_ms(5);

                        if(minute_backup != min)
                        {
                                last_minute = min;
                        }
                }

                /* ---- Threshold check: temperature vs set point ----
                   If exceeded: blink buzzer every loop, and send a cloud
                   alert once on the rising edge (not every iteration).   */
                if(temp_c > (int)set_point)   /* both in degC (was volts vs degC) */
                {
                        buzzer_alert();

                        if(over_temp_prev == 0)
                        {
                                update_data(2, temp_c);
                                over_temp_prev = 1;
                        }
                }
                else
                {
                        IOCLR0 = (1 << TEMP_BUZZER);
                        over_temp_prev = 0;
                }
        }
}
/* MAIN function ends here */

/* External interrupt service routine (local set-point switch) */
void sw_pressed(void) __irq
{
        interrupt_flag = 1;

        /* Clear interrupt flag */
        EXTINT = (1 << 0);

        /* End of interrupt */
        VICVectAddr = 0;
}
