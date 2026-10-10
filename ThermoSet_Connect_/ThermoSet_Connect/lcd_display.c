#include<lpc21xx.h>
#include<stdio.h>
#include "delay.h"
#include "lcd_defines.h"
#include "cust_lcd.h"
#include "rtc.h"
#include "i2c.h"
#include "eeprom.h"
#include "esp01.h"
#include "uart.h"
#include "keypad_defines.h"
#include "menu.h"

#define SETPOINT_ADDR 0x77

/* Local set-point / time editing menu, triggered by the switch interrupt */
void menu(unsigned int *temp, int *sec, int *min, int *hour,int *day, int *mon, int *year, int *dow)
{
        int ch, tmenu,dtemp,itemp;

        while (1)
        {
                cmd_lcd(0x01);

                cmd_lcd(0x80);
                string_lcd("1.EDIT  2.SP");

                cmd_lcd(0xc0);
                string_lcd("3.DISPSP  4.EXIT");

                while (col_stat());

                ch = key_val();

                if (ch == 1)
                {
                        while (!col_stat());

                        while (1)
                        {
                                cmd_lcd(0x01);

                                cmd_lcd(0x80);
                                string_lcd("1.H 2.M 3.S 4.D");
                                cmd_lcd(0xc0);
                                string_lcd("5.M6.Y 7.DAY 8.E");

                                while (col_stat());

                                tmenu = key_val();

                                if (tmenu == 1)
                                {
                                        while (!col_stat());

                                        cmd_lcd(0x01);
                                        cmd_lcd(0x80);
                                        string_lcd("hour(0-23)");

                                        *hour = ip_keypad(0,24);

                                        if (*hour > 23)
                                        {
                                                cmd_lcd(0x01);
                                                cmd_lcd(0x80);
                                                string_lcd("out of range");
                                                cmd_lcd(0xc0);
                                                string_lcd("range is (0-23)");

                                                delay_ms(1000);
                                        }
                                        else if((*hour)!=-1)
                                        {
                                                 set_time_info(sec,min,hour);
                                        }
                                }

                               else if (tmenu == 2)
                                {
                                        while (!col_stat());

                                        cmd_lcd(0x01);
                                        cmd_lcd(0x80);
                                        string_lcd("enter min");

                                        *min = ip_keypad(0,60);

                                        if (*min > 59)
                                        {
                                                cmd_lcd(0x01);
                                                cmd_lcd(0x80);
                                                string_lcd("out of range");
                                                cmd_lcd(0xc0);
                                                string_lcd("range is (0-59)");
                                                delay_ms(300);
                                        }
                                        else if((*min)!=-1)
                                        {
                                                 set_time_info(sec,min,hour);
                                        }
                                }

                                else if (tmenu == 3)
                                {
                                        while (!col_stat());

                                        cmd_lcd(0x01);
                                        cmd_lcd(0x80);
                                        string_lcd("enter seconds");

                                        *sec = ip_keypad(0,60);

                                        if (*sec > 59)
                                        {
                                                cmd_lcd(0x01);
                                                cmd_lcd(0x80);
                                                string_lcd("out of range");
                                                cmd_lcd(0xc0);
                                                string_lcd("range is (0-59)");
                                                delay_ms(300);
                                        }
                                        else if((*sec)!=-1)
                                        {
                                                set_time_info(sec,min,hour);
                                        }
                                }

                                else if (tmenu == 4)
                                {
                                        while (!col_stat());

                                        cmd_lcd(0x01);
                                        cmd_lcd(0x80);
                                        string_lcd("enter date");

                                        *day = ip_keypad(1,31);

                                        if (*day > 31)
                                        {
                                                cmd_lcd(0x01);
                                                cmd_lcd(0x80);
                                                string_lcd("out of range");
                                                cmd_lcd(0xc0);
                                                string_lcd("range is (1-31)");
                                                delay_ms(300);
                                        }
                                        else if((*day)!=-1)
                                        {
                                                set_date_info(day, mon, year);
                                        }
                                }

                                else if (tmenu == 5)
                                {
                                        while (!col_stat());

                                        cmd_lcd(0x01);
                                        cmd_lcd(0x80);
                                        string_lcd("enter month");

                                        *mon = ip_keypad(1,12);

                                        if (*mon > 12)
                                        {
                                                cmd_lcd(0x01);
                                                cmd_lcd(0x80);
                                                string_lcd("out of range");
                                                cmd_lcd(0xc0);
                                                string_lcd("range is (1-12)");
                                                delay_ms(300);
                                        }
                                        else if((*mon)!=-1)
                                        {
                                                set_date_info(day, mon, year);
                                        }
                                }

                                else if (tmenu == 6)
                                {
                                        while (!col_stat());

                                        cmd_lcd(0x01);
                                        cmd_lcd(0x80);
                                        string_lcd("enter year");

                                        *year = ip_keypad(0,9999);

                                        set_date_info(day, mon, year);
                                }

                                else if (tmenu == 7)
                                {
                                        while (!col_stat());

                                        cmd_lcd(0x01);
                                        cmd_lcd(0x80);
                                        string_lcd("enter week(0-6)");

                                        *dow = ip_keypad(0,6);

                                        if (*dow > 6)
                                        {
                                                cmd_lcd(0x01);
                                                cmd_lcd(0x80);
                                                string_lcd("out of range");
                                                cmd_lcd(0xc0);
                                                string_lcd("range is (0-6)");
                                                delay_ms(300);
                                        }
                                        else if((*dow)!=-1)
                                        {
                                                set_week(dow);
                                        }
                                }

                                if (tmenu == 8)
                                {
                                        break;
                                }
                        }
                }

                else if (ch == 2)
                {
                        while (!col_stat());

                        cmd_lcd(0x01);
                        cmd_lcd(0x80);
                        string_lcd("enter setpoint");

                        itemp = ip_keypad(0,150);
                        if(itemp==-1)
                                continue;
                        else if(itemp>150)
                        {
                                cmd_lcd(0x01);
                                cmd_lcd(0x80);
                                string_lcd("out of range");
                                cmd_lcd(0xc0);
                                string_lcd("range is (0-150)");
                                delay_ms(300);
                        }
                        else
                           {
                                *temp=itemp;
                                 cmd_lcd(0x01);
                                 string_lcd("Saving to");
                                 cmd_lcd(0xc0);
                                 string_lcd("EEPROM...");
                                 /* Persist the new set point so it survives a power cycle */
                                 byte_write(SA,SETPOINT_ADDR,(char)(*temp));
                                 /* Log the new set point to the cloud (field 3) */
                                 update_data(3,(int)(*temp));
                                  cmd_lcd(0x01);
                             }
                }

                else if (ch == 3)
                {
                        while (!col_stat());
                        cmd_lcd(0x01);
                        while(1)
                        {
                                while(!col_stat());
                                cmd_lcd(0x80);
                                string_lcd("setpoint=");
                                int_lcd((*temp));
                                data_lcd(0);
                                data_lcd('C');
                                cmd_lcd(0xc0);
                                string_lcd("#.EXIT");
                                while(col_stat());
                                dtemp=key_val();
                                while(!col_stat());
                                if(dtemp=='#')
                                        break;
                        }
                }
                else if (ch == 4)
                {
                        while (!col_stat());
                        break;
                }
        }
}

/* Generic numeric-entry helper used by the menu above */
int ip_keypad(int min,int max)
{
        int val = 0;
        int stop;
        int typed=0;

        cmd_lcd(0xc0);

        while (1)
        {
                while (col_stat());

                stop = key_val();

                while (!col_stat());

                if (stop == '#')
                        break;
                if(stop=='D')
                {
                        val=val/10;

                        cmd_lcd(0xc0);
                        string_lcd("                ");
                        cmd_lcd(0xc0);
                        int_lcd(val);
                        if(typed>0)
                                typed--;
                        continue;
                }

                val = val * 10 + stop;
                if(val>max)
                {
                        cmd_lcd(0x01);
                        cmd_lcd(0x80);
                        string_lcd("out of range");
                        break;
                }
                cmd_lcd(0xc0);
                string_lcd("                ");

                cmd_lcd(0xc0);
                int_lcd(val);
                typed++;
        }

        if(typed==0)
                return -1;
        else
                return val;
}
