#include<lpc21xx.h>

/* Keypad row pin definitions */
#define R0 16
#define R1 17
#define R2 18
#define R3 19

/* Keypad column pin definitions */
#define C0 20
#define C1 21
#define C2 22
#define C3 23

/* #include"types.h" */

/* Function declarations */

/* Initialize keypad */
void init_keypad(void);

/* Check column status */
char col_stat(void);

/* Get pressed key value */
char key_val(void);
