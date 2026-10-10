#include<lpc21xx.h>
#define  AIN0 0
#define  AIN1 1
#define  AIN2 2
#define  AIN3 3
#define  AIN4 4
#define  AIN5 5
#define  AIN6 6
#define  AIN7 7
#define FOSC 12000000
#define CCLK (5*FOSC)
#define PCLK (CCLK/4)
#define ADC_CLK 3000000
#define CLK_DIV ((PCLK/ADC_CLK)-1)
#define BURST 16
#define CLKDIV_POS 8
#define PDN 21
#define START 24
#define RESULT 6
#define DONE 31
void init_adc(void);
void read_adc(int chno,int *dval,float *aval);
void temp_display(float );
