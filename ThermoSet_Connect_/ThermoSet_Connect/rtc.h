#include<lpc21xx.h>

/* Oscillator frequency */
#define FOSC 12000000

/* CPU clock frequency */
#define CCLK (5*FOSC)

/* Peripheral clock frequency */
#define PCLK (CCLK/4)

/* RTC prescaler integer value */
#define PRE_INT ((int)(PCLK/32768)-1)

/* RTC prescaler fractional value */
#define PRE_FRAC (PCLK-((PRE_INT)+1)*32768)

/* RTC control bit definitions */
#define CLK_EN  0
#define CLK_SRC 4
#define CLK_RST 1

/* Function declarations */

/* Initialize RTC module */
void init_rtc(void);

/* Set RTC time */
void set_time_info(int *,int *,int *);

/* Set RTC date */
void set_date_info(int *,int *,int *);

/* Set RTC week day */
void set_week(int *w);

/* Read RTC date and time */
void get_info(int *,int *,int *,int *,int *,int *,int *);

/* Display RTC information on LCD */
void display_info(int s,int m,int h,int ,int ,int ,int );
