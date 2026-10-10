#include<lpc21xx.h>

/* External interrupt channel definitions (local set-point switch) */
#define EINT0_CH  14
#define EINT0_PIN 16

/* Function declarations */

/* Initialize switch interrupt (used for local set-point entry) */
void init_interrupt(void);

/* ISR for switch press */
void sw_pressed(void) __irq;
