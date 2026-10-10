#include<lpc21xx.h>
#include "eeprom.h"
#include "i2c.h"
#include "delay.h"

/* Function to write single byte into EEPROM */
void byte_write(char addr,short int baddr,char ch)
{
        /* Generate start condition */
        start();

        /* Send slave address with write bit */
        write(addr<<1);

        /* Send higher byte address */
        write(baddr>>8);

        /* Send lower byte address */
        write(baddr);

        /* Send data byte */
        write(ch);

        /* Generate stop condition */
        stop();

        /* EEPROM internal write delay */
        delay_ms(10);
}

/* Function to read single byte from EEPROM */
char byte_read(char addr,short int baddr)
{
        char store;

        /* Start condition */
        start();

        /* Send slave address with write bit */
        write(addr<<1);

        /* Send higher byte address */
        write(baddr>>8);

        /* Send lower byte address */
        write(baddr);

        /* Repeated start condition */
        rep_start();

        /* Send slave address with read bit */
        write(addr<<1|1);

        /* Read data with NACK */
        store=nack();

        /* Stop condition */
        stop();

        return store;
}

/* Function to write multiple bytes into EEPROM */
void page_write(char addr,short int baddr,char *p)
{
        /* Start condition */
        start();

        /* Send slave address */
        write(addr<<1);

        /* Send higher byte address */
        write(baddr>>8);

        /* Send lower byte address */
        write(baddr);

        /* Write data continuously */
        while(*p)
        {
                write(*p);

                p++;
        }

        /* Write NULL character */
        write(*p);

        /* Stop condition */
        stop();

        /* EEPROM write delay */
        delay_ms(10);
}

/* Function to read sequential bytes from EEPROM */
void seq_read(char addr,short int baddr,char *ch)
{
        int i,j=0;

        ch[0]='a';

        /* Start condition */
        start();

        /* Send slave address */
        write(addr<<1);

        /* Send higher byte address */
        write(baddr>>8);

        /* Send lower byte address */
        write(baddr);

        /* Repeated start */
        rep_start();

        for(i=0;ch[i];i++)
        {
                /* Read byte with ACK */
                ch[j++]=mack();
        }

        /* Last byte read with NACK */
        nack();

        /* Stop condition */
        stop();
} 
