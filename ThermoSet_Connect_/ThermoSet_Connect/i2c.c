#include<lpc21xx.h>
#include "i2c.h"

/* Function to initialize I2C peripheral */
void init_i2c()
{
        /* Configure SDA and SCL pins */
        PINSEL0 = PINSEL0 & ~(15<<4);

        PINSEL0 = PINSEL0 | (1<<(SCL)*2) | (1<<(SDA*2));

        /* Set clock high and low values */
        I2SCLH = D_VAL;
        I2SCLL = D_VAL;

        /* Enable I2C */
        I2CONSET = 1<<I2EN;
}

/* Function to generate START condition */
void start()
{
        /* Set START bit */
        I2CONSET = 1<<STA;

        /* Wait for SI flag */
        while(((I2CONSET>>SI)&1)==0);

        /* Clear START flag */
        I2CONCLR = 1<<STAC;
}

/* Function to generate STOP condition */
void stop()
{
        /* Set STOP bit */
        I2CONSET = 1<<STO;

        /* Clear interrupt flag */
        I2CONCLR = 1<<SIC;
}

/* Function to transmit one byte */
void write(char ch)
{
        /* Load data into I2DAT register */
        I2DAT = ch;

        /* Clear SI flag */
        I2CONCLR = 1<<SIC;

        /* Wait for transmission complete */
        while(((I2CONSET>>SI)&1)==0);
}

/* Function to generate repeated START */
void rep_start()
{
        /* Set START bit */
        I2CONSET = 1<<STA;

        /* Clear SI flag */
        I2CONCLR = 1<<SIC;

        /* Wait for SI flag */
        while(((I2CONSET>>SI)&1)==0);

        /* Clear START flag */
        I2CONCLR = 1<<STAC;
}

/* Function to receive byte with ACK */
char mack()
{
        /* Enable acknowledge */
        I2CONSET = 1<<AA;

        /* Clear SI flag */
        I2CONCLR = 1<<SIC;

        /* Wait for data reception */
        while(((I2CONSET>>SI)&1)==0);

        /* Clear ACK flag */
        I2CONCLR = 1<<AAC;

        return I2DAT;
}

/* Function to receive byte with NACK */
char nack()
{
        /* Clear SI flag */
        I2CONCLR = 1<<SIC;

        /* Wait until data received */
        while(((I2CONSET>>SI)&1)==0);

        return I2DAT;
}
