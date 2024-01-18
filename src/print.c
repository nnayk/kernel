/*
 * Nakul Nayak
 * Kernel Project
 * Description: 
 * Print function definitions
*/

#include "vga.h"

#define NUM_ASCII_OFF 48

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
        unsigned char digits[3]; 
        int index = 0;
        while(b)
        {
                temp = (b%10)+NUM_ASCII_OFF;
                digits[index++] = temp;
                b=b/10;
        }
        while(index--) VGA_display_char(digits[index]);
}
#if 0
void print_short(short);
void print_long_hex(long);
#endif
