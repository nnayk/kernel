/*
 * Nakul Nayak
 * Kernel Project
 * Description: 
 * Print function definitions
*/

#include "vga.h"

#define NUM_ASCII_OFF 48
#define MAX_UCHAR_LEN 3
#define MAX_SHORT_LEN 5
#define MINUS_ASCII 45

void print_char(char c)
{
        VGA_display_char(c);        
}

void print_str(const char *str)
{
        VGA_display_str(str);
}
void print_uchar(unsigned char b)
{
        unsigned char temp = 0;
        unsigned char digits[MAX_UCHAR_LEN]; 
        int index = 0;
        while(b)
        {
                temp = (b%10)+NUM_ASCII_OFF;
                digits[index++] = temp;
                b=b/10;
        }
        while(index--) VGA_display_char(digits[index]);
}

void print_short(short s)
{
        unsigned char temp = 0;
        unsigned char digits[MAX_SHORT_LEN]; 
        int index = 0;
        int loop = 0;
        // this cleanly handles the case where we're given the max. negative short value
        int s_copy = s; 
        if(s_copy < 0) 
        {
                VGA_display_char(MINUS_ASCII);
                /*
                if(max_negative)
                {
                        digits[index++] = 9;
                        s++;
                }
                */
                s_copy=s_copy*-1;
        }
        //while(!loop);
        while(s_copy)
        {
                temp = (s_copy%10)+NUM_ASCII_OFF;
                digits[index++] = temp;
                s_copy=s_copy/10;
        }
        while(index--) VGA_display_char(digits[index]);
}

#if 0
void print_long_hex(long);
#endif
