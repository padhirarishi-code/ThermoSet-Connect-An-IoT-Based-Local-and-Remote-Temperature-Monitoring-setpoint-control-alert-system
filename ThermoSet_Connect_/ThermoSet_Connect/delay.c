#include "delay.h"

/* Function to generate microsecond delay */
void delay_us(unsigned int tdly)
{
        /* Convert into processor clock cycles */
        tdly*=12;

        /* Wait loop */
        while(tdly--);
}

/* Function to generate millisecond delay */
void delay_ms(unsigned int tdly)
{
        /* Convert milliseconds into clock cycles */
        tdly*=12000;

        /* Wait loop */
        while(tdly--);
}

/* Function to generate second delay */
void delay_s(unsigned int tdly)
{
        /* Convert seconds into clock cycles */
        tdly*=12000000;

        /* Wait loop */
        while(tdly--);
}
