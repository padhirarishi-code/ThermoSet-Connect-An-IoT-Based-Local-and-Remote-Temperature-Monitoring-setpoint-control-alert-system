#include <LPC21xx.H>

#include "uart.h"
#include "lcd_defines.h"
char buff[300];
int i=0;

/* Function to initialize UART0 */
void init_uart (void)
{
  /* Configure P0.0 as TXD0 and P0.1 as RXD0 */
  PINSEL0 = PINSEL0&~(15<<0);
  PINSEL0 = PINSEL0|(5<<0);

  /* Enable DLAB bit */
  U0LCR = 0x83;

  /* Set baud rate value for 9600 */
  U0DLL = 97 ;
  U0DLM = 0;

  /* 8-bit data, 1 stop bit, no parity */
  U0LCR = 0x03;

  #if UART_INT_ENABLE > 0

  /* Configure UART interrupt */
  VICIntSelect = 0x00000000;
  VICVectAddr0 = (unsigned)UART0_isr;
  VICVectCntl0 = 0x20 | 6;
  VICIntEnable = 1 << 6;

  /* Enable UART interrupts */
  U0IER = 0x03;

  #endif
}
void UART0_isr(void)__irq
{
if(U0IIR &0x04)
{

{
        char c = U0RBR;
        if(i < 299)
                buff[i++] = c;
}


}
VICVectAddr=0;
}

/* Function to transmit single character */
void tx_uart(char ch)
{
        U0THR = ch;

        /* Wait until transmission completes */
        while (!(U0LSR & 0x20));
}

/* Function to receive single character */
char rx_uart(void)
{
        /* Wait until data received */
        while (!(U0LSR & 0x01));

        return (U0RBR);
}

/* Function to transmit string */
void string_uart(char *s)
{
   while(*s)
   {
       tx_uart(*s);
       s++;
   }
}

/* Function to transmit integer value */
void int_uart(unsigned int num)
{
        int rev=0,temp,zero=0;

        temp=num;

        /* If number is zero */
        if(num==0)
        {
                tx_uart('0');
                return ;
        }

        /* Check trailing zeros */
        if((temp%1000)==0)
        {
                zero=3;
        }
        else if((temp%100)==0)
        {
                zero=2;
        }
        else if((temp%10)==0)
        {
                zero=1;
        }

        /* Reverse the number */
        for(;temp!=0;temp/=10)
        {
                rev=rev*10+(temp%10);
        }

        temp=rev;

        /* Print integer */
        for(;temp!=0;temp/=10)
        {
                tx_uart((temp%10)+'0');
        }

        /* Print remaining zeros */
        while(zero--)
        {
                tx_uart('0');
        }
}

/* Function to transmit floating point value */
void float_uart(float f)
{
        int x;
        float temp;

        x=f;

        int_uart(x);

        tx_uart('.');

        temp=(f-x)*100;

        x=temp;

        int_uart(x);
}

/* Function to display time through UART */
void disp_time(int h,int m,int s)
{
        string_uart("Time:");

        int_uart(h);
        tx_uart(':');

        int_uart(m);
        tx_uart(':');

        int_uart(s);

        string_uart("\r\n");
}
