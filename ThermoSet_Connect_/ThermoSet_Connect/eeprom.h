#include<lpc21xx.h>

/* EEPROM slave address */
#define SA 0X50

/* Function declarations */

/* Write single byte into EEPROM */
void byte_write(char addr,short int baddr,char ch);

/* Read single byte from EEPROM */
char byte_read(char addr,short int baddr);

/* Write multiple bytes into EEPROM */
void page_write(char addr,short int baddr,char *p);

/* Read sequential bytes from EEPROM */
void seq_read(char addr,short int baddr,char *ch);
