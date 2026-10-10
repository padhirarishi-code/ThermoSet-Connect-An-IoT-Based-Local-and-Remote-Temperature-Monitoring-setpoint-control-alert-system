#include "clock.h"
#define UART_INT_ENABLE 1
void UART0_isr(void) __irq;
/* Function declarations */

/* Initialize UART peripheral */
void init_uart(void);

/* Transmit single character through UART */
void tx_uart(char ch);

/* Receive single character from UART */
char rx_uart(void);

/* Transmit string through UART */
void string_uart(char *);

/* Transmit integer value through UART */
void int_uart(unsigned int);

/* Transmit floating point value through UART */
void float_uart(float);

/* Display time through UART */
void disp_time(int h,int m,int s);
