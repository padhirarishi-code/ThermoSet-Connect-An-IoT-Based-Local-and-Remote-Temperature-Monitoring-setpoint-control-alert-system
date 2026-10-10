#include "rtc.h"
#include "lcd_defines.h"

/* Week day names */
char week[][4]={"sun","mon","tue","wed","thu","fri","sat"};

/* Function to initialize RTC */
void init_rtc()
{
        /* Reset RTC clock */
        CCR = 1<<CLK_RST;

        #ifdef _LPC2148

        /* Enable RTC and select clock source */
        CCR = (1<<CLK_EN) | (1<<CLK_SRC);

        #else

        /* Load RTC prescaler values */
        PREINT  = PRE_INT;
        PREFRAC = PRE_FRAC;

        /* Enable RTC */
        CCR = 1<<CLK_EN;

        #endif
}

/* Function to set time */
void set_time_info(int *s,int *m,int *h)
{
        HOUR = *h;
        MIN  = *m;
        SEC  = *s;
}

/* Function to set date */
void set_date_info(int *d,int *mo,int *y)
{
        DOM   = *d;
        MONTH = *mo;
        YEAR  = *y;
}

/* Function to set week day */
void set_week(int *w)
{
        DOW = *w;
}

/* Function to get RTC information */
void get_info(int *s,int *m,int *h,int *d,int *mo,int *y,int *w)
{
        *h  = HOUR;
        *m  = MIN;
        *s  = SEC;

        *d  = DOM;
        *mo = MONTH;
        *y  = YEAR;

        *w  = DOW;
}

/* Function to display date and time on LCD */
void display_info(int s,int m,int h,int d,int mo,int y,int w)
{
                /* Move cursor to first row */
                cmd_lcd(0x80);

                /* Display hour */
                char_lcd((h/10)+'0');
                char_lcd((h%10)+'0');

                char_lcd(':');

                /* Display minute */
                char_lcd((m/10)+'0');
                char_lcd((m%10)+'0');

                char_lcd(':');

                /* Display seconds */
                char_lcd((s/10)+'0');
                char_lcd((s%10)+'0');

                /* Move cursor to second row */
                cmd_lcd(0xc0);

                /* Display date */
                char_lcd((d/10)+'0');
                char_lcd((d%10)+'0');

                char_lcd('/');

                /* Display month */
                char_lcd((mo/10)+'0');
                char_lcd((mo%10)+'0');

                char_lcd('/');

                /* Display year */
                int_lcd(y%100);

                /* Week display optional */
                //string_lcd(week[w]);
}
