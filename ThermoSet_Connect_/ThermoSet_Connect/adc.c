#include<lpc21xx.h>
#include "adc.h"
#include "delay.h"
#include "lcd_defines.h"
void init_adc()
{
        PINSEL1=PINSEL1&~(3<<2*(28-16));
        PINSEL1=PINSEL1|(1<<2*(28-16));
        ADCR=(1<<PDN)|(CLK_DIV<<CLKDIV_POS);
}
void read_adc(int chno,int *dval,float *aval)
{
        ADCR&=~0XFF;
        ADCR|=(1<<chno);
        ADCR|=1<<START;
        delay_us(3);
        while(((ADDR>>DONE)&1)==0);
        ADCR&=~(1<<START);
		*dval=((ADDR>>RESULT)&0X3FF);
        *aval=(*dval)*(3.3/1023);
        //ADCR&=~(7<<START);
}
void temp_display(float aval)
{
                int temp;
                cmd_lcd(0x89);
                char_lcd('T');
                char_lcd(':');
                temp=aval*100;
                int_lcd(temp);
                char_lcd(0);
                char_lcd('C');
}
