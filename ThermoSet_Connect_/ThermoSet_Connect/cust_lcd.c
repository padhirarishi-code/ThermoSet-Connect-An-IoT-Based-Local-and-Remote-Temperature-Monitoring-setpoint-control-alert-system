#include <lpc21xx.h>
#include "lcd_defines.h"

/* Function to create custom LCD character */
void cust_lcd(void)
{
        /* Custom character pattern */
        char custom_symbol[] =
        {
                0x0C,
                0x12,
                0x12,
                0x0C,
                0x00,
                0x00,
                0x00,
                0x00
        };

        /* Move to CGRAM location */
        cmd_lcd(0x40);

        /* Store custom character */
        cus_char(custom_symbol);
}
