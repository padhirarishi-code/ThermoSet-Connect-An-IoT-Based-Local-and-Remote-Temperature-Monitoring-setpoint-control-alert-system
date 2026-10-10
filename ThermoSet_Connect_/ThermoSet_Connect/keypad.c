#include<LPC21xx.h>
#include"keypad_defines.h"

/* Lookup table for keypad values */
char LUT[][4]={
                   1,2,3,'A',
                   4,5,6,'B',
                   7,8,9,'C',
                   '*',0,'#','D'
               };

/* Function to initialize keypad */
void init_keypad(void)
{
                /* Configure row pins as output */
                IODIR1|=((1<<R0)|(1<<R1)|(1<<R2)|(1<<R3));

                /* P1.16-P1.19 are configured as output pins */

                /* Initialize all rows to LOW */
                IOCLR1=((1<<R0)|(1<<R1)|(1<<R2)|(1<<R3));
}

/* Function to check column status */
char col_stat(void)
{
        if((((IOPIN1>>20)&0x0f)==0x0f))
                return 1;
        else
                return 0;
}

/* Function to identify pressed key value */
char key_val(void)
{
                char row_val=0,col_val=0;

                /* Make Row0 LOW and remaining rows HIGH */
                IOCLR1=(1<<R0);
                IOSET1=((1<<R1)|(1<<R2)|(1<<R3));

                if((((IOPIN1>>20)&0x0f)!=0x0f))
                {
                        row_val=0;
                        goto colcheck;
                }

                /* Make Row1 LOW and remaining rows HIGH */
                IOCLR1=(1<<R1);
                IOSET1=((1<<R0)|(1<<R2)|(1<<R3));

                if((((IOPIN1>>20)&0x0f)!=0x0f))
                {
                        row_val=1;
                        goto colcheck;
                }

                /* Make Row2 LOW and remaining rows HIGH */
                IOCLR1=(1<<R2);
                IOSET1=((1<<R0)|(1<<R1)|(1<<R3));

                if((((IOPIN1>>20)&0x0f)!=0x0f))
                {
                        row_val=2;
                        goto colcheck;
                }

                /* Make Row3 LOW and remaining rows HIGH */
                IOCLR1=(1<<R3);
                IOSET1=((1<<R0)|(1<<R1)|(1<<R2));

                if((((IOPIN1>>20)&0x0f)!=0x0f))
                        row_val=3;

                /* Column checking */
                colcheck:

                if(((IOPIN1>>C0)&1)==0)
                        col_val=0;

                else if(((IOPIN1>>C1)&1)==0)
                        col_val=1;

                else if(((IOPIN1>>C2)&1)==0)
                        col_val=2;

                else
                        col_val=3;

                /* Clear all rows */
                IOCLR1=((1<<R0)|(1<<R1)|(1<<R2)|(1<<R3));

                /* Return corresponding keypad value */
                return(LUT[row_val][col_val]);
}
