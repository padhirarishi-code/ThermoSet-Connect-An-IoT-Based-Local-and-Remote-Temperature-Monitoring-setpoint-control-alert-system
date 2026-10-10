#include "lcd_defines.h"
#include "delay.h"

/* LCD initialization function */
void init_lcd(void)
{
        /* Configure control pins as output */
        IODIR0 |= (1 << RS) | (1 << RW) | (1 << EN);

        /* Configure data pins as output */
        IODIR0 |= (0xFF << LED);

        delay_ms(20);

        /* LCD initialization sequence */
        cmd_lcd(0x30);
        delay_ms(8);

        cmd_lcd(0x30);
        delay_ms(4);

        cmd_lcd(0x30);
        delay_ms(4);

        /* 8-bit mode */
        cmd_lcd(0x38);

        /* Cursor move */
        cmd_lcd(0x10);

        /* Clear display */
        cmd_lcd(0x01);

        /* Entry mode */
        cmd_lcd(0x06);

        /* Display ON */
        cmd_lcd(0x0C);
}

/* Send command to LCD */
void cmd_lcd(char value)
{
        /* Clear previous data */
        IOCLR0 = (0xFF << LED);

        /* RS = 0 for command */
        IOCLR0 = (1 << RS);

        /* Place command on data pins */
        IOSET0 = (value << LED);

        /* Enable pulse */
        IOSET0 = (1 << EN);

        delay_ms(2);

        IOCLR0 = (1 << EN);
}

/* Send data to LCD */
void data_lcd(char value)
{
        /* Clear previous data */
        IOCLR0 = (0xFF << LED);

        /* RS = 1 for data */
        IOSET0 = (1 << RS);

        /* RW = 0 for write */
        IOCLR0 = (1 << RW);

        /* Send data */
        IOSET0 = (value << LED);

        /* Enable pulse */
        IOSET0 = (1 << EN);

        delay_ms(2);

        IOCLR0 = (1 << EN);
}

/* Display single character */
void char_lcd(char ch)
{
        data_lcd(ch);
}

/* Display integer on LCD */
void int_lcd(int number)
{
        int reverse_num = 0;
        int copy_value;
        int trailing_zero = 0;

        copy_value = number;

        /* If number is zero */
        if(number == 0)
        {
                char_lcd('0');
                return;
        }

        /* Check trailing zeros */
        if((copy_value % 1000) == 0)
        {
                trailing_zero = 3;
        }
        else if((copy_value % 100) == 0)
        {
                trailing_zero = 2;
        }
        else if((copy_value % 10) == 0)
        {
                trailing_zero = 1;
        }

        /* Reverse the number */
        while(copy_value > 0)
        {
                reverse_num = (reverse_num * 10) + (copy_value % 10);

                copy_value = copy_value / 10;
        }

        copy_value = reverse_num;

        /* Print digits */
        while(copy_value > 0)
        {
                char_lcd((copy_value % 10) + '0');

                copy_value = copy_value / 10;
        }

        /* Print remaining zeros */
        while(trailing_zero > 0)
        {
                char_lcd('0');

                trailing_zero--;
        }
}

/* Display string on LCD */
void string_lcd(char *ptr)
{
        while(*ptr != '\0')
        {
                char_lcd(*ptr);

                ptr = ptr + 1;
        }
}

/* Create custom character */
void cus_char(char *arr)
{
        int index = 0;

        while(index < 8)
        {
                char_lcd(arr[index]);

                index++;
        }
}
