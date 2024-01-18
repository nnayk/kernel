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
#define FMT_SPEC %
// format specifier constants below
#define FMT_MODULO %
#define FMT_INT d
#define FMT_UCHAR u
#define FMT_INT d
#define FMT_CHAR c
#define FMT_CHAR p // note: a pointer can be treated as a long for x86_64
#define FMT_STR s
// TODO: define format constants for the 3 bracket cases listed in instructions

__attribute__ ((format (printf, 1, 2)))
int printk(const char *str,...)
{
        if(!str) return ERR_NULL_PTR;
        size_t len = strlen(str);
        for(int i=0;i<len;i++)
        {
                if(str[i] == FORMAT_SPEC)
                {
                        // TODO: handle format specifier
                }
                // just a regular char, display it
                VGA_display_char(str[i]);
        }

        return 1;
}
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
        // this cleanly handles the case where we're given the max. negative short value
        int s_copy = s; 
        if(s_copy < 0) 
        {
                VGA_display_char(MINUS_ASCII);
                s_copy=s_copy*-1;
        }
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
