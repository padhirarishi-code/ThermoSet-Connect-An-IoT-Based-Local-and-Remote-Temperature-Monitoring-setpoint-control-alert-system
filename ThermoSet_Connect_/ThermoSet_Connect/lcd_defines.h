#include <lpc21xx.h>

/* LCD control pin definitions */
#define RS   18
#define RW   20
#define EN   19

/* LCD data pin starting position */
#define LED 8

/* Function declarations */

/* Initialize LCD */
void init_lcd(void);

/* Send command to LCD */
void cmd_lcd(char value);

/* Send data to LCD */
void data_lcd(char value);

/* Display single character */
void char_lcd(char ch);

/* Display integer value */
void int_lcd(int number);

/* Display string */
void string_lcd(char *ptr);

/* Create/display custom character */
void cus_char(char *arr);
