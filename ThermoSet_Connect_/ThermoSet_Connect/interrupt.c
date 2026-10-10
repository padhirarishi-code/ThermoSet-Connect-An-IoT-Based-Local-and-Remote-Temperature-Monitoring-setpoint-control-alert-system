#include<lpc21xx.h>
#include "interrupt.h"

/* Function to initialize external interrupt (local set-point switch) */
void init_interrupt()
{
        /* Configure EINT0 pin function */
        PINSEL1 = PINSEL1 & ~(3<<(16-EINT0_PIN));

        PINSEL1 = PINSEL1 | (1<<(16-EINT0_PIN));

        /* Enable EINT0 interrupt */
        VICIntEnable |= (1<<EINT0_CH);

        /* Configure vector control register */
        VICVectCntl1 = (EINT0_CH) | (1<<5);

        /* Load ISR address */
        VICVectAddr1 = (unsigned int)sw_pressed;

        /* Clear pending interrupt */
        EXTINT = 1<<0;

        /* Configure edge sensitive mode */
        EXTMODE = 1<<0;

        /* Configure falling edge trigger */
        EXTPOLAR = 0<<0;
}
