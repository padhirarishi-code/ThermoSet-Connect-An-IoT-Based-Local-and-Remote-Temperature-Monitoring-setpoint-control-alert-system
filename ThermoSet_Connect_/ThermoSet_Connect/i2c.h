#include<lpc21xx.h>
#include "clock.h"

/* I2C pin definitions */
#define SCL 2
#define SDA 3

/* I2C control bit definitions */
#define I2EN 6
#define STA  5
#define STO  4
#define SI   3
#define AA   2

/* I2C clear bit definitions */
#define I2ENC
#define STAC 5
#define SIC  3
#define AAC  2

/* I2C clock frequency */
#define S_CLK 100000

/* I2C delay calculation value */
#define D_VAL ((PCLK/S_CLK)/2)

/* Function declarations */

/* Initialize I2C module */
void init_i2c(void);

/* Generate START condition */
void start(void);

/* Generate STOP condition */
void stop(void);

/* Write one byte through I2C */
void write(char ch);

/* Generate repeated START */
void rep_start(void);

/* Read byte with ACK */
char mack(void);

/* Read byte with NACK */
char nack(void);
