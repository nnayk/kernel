/*
 * Nakul Nayak
 * Kernel Project
 * Description: 
 * Print function definitions
*/

#include <stdarg.h>
#include <limits.h>
#include "vga.h"

#define NUM_ASCII_OFF 48
#define CHAR_ASCII_OFF 65
#define ARR_SIZE 20 // should handle all data type cases
#define MINUS_ASCII 45
#define DEC_BASE 10
#define NEG_DEC_BASE -DEC_BASE
#define HEX_BASE 16
#define HEX_PREFIX "0x"
#define FMT_SPEC %
// format specifier constants below
#define FMT_MODULO %
#define FMT_INT d
#define FMT_UCHAR u
#define FMT_INT d
#define FMT_CHAR c
#define FMT_PTR p // note: a pointer can be treated as a long for x86_64
#define FMT_STR s
// TODO: define format constants for the 3 bracket cases listed in instructions

__attribute__ ((format (printf, 1, 2)))
int printk(const char *str,...)
{
        /*
        va_list va;
        va_start(va, fmt);
        int int_val = va_arg(va, int);
        void *voidp_val = va_arg(va, void*);
        if(!str) return ERR_NULL_PTR;
        size_t len = strlen(str);
        for(int i=0;i<len;i++)
        {
                if(str[i] == FMT_SPEC)
                {

                        // TODO: handle format specifiers
                }
                // just a regular char, display it
                VGA_display_char(str[i]);
        }

        va_end(va);
        */
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
        unsigned char digits[ARR_SIZE]; 
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
        unsigned char digits[ARR_SIZE]; 
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

void print_long_decimal(long l)
{
        unsigned char digits[ARR_SIZE]; 
        int index = 0;
        unsigned char temp = 0;

        if(l<0)
        {
                VGA_display_char(MINUS_ASCII);
                if(l==LONG_MIN)
                {
                        digits[index++] = -1*(l%NEG_DEC_BASE);
                }
                l=l*-1;
        }
        while(l)
        {
                temp = (l%DEC_BASE)+NUM_ASCII_OFF;
                digits[index++] = temp;
                l=l/DEC_BASE;
        }
        while(index--) VGA_display_char(digits[index]);
}

void print_long_hex(unsigned long l)
{
        unsigned char digits[ARR_SIZE]; 
        int index = 0;
        unsigned char temp;
        VGA_display_str(HEX_PREFIX);
        while(l)
        {
                temp = (l%HEX_BASE);
                if(temp >= 10)
                {
                        temp = temp + CHAR_ASCII_OFF;
                }
                else temp = temp + NUM_ASCII_OFF;
                digits[index++] = temp;
                l=l/HEX_BASE;
        }
        while(index--) VGA_display_char(digits[index]);
}
