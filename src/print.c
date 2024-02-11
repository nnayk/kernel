/*
 * Nakul Nayak
 * Kernel Project
 * Description: 
 * Print function definitions
*/

#include <stdarg.h>
#include <stddef.h>
#include <limits.h>
#include "vga.h"
#include "utility.h"
#include "print.h"
#include "error.h"
#include "shared_buff.h"
#include "serial.h"

//offsets
#define NUM_ASCII_OFF 48
#define CHAR_ASCII_OFF 65
#define ARR_SIZE 20 // should handle all data type cases for x86_64
#define MINUS_ASCII 45

//decimal and hex constants
#define DEC_BASE 10
#define NEG_DEC_BASE -DEC_BASE
#define HEX_BASE 16
#define HEX_PREFIX "0x"

// format specifier constants below
#define FMT_SPEC '%'
#define FMT_MODULO '%'
#define FMT_INT 'd'
#define FMT_UINT 'u'
#define FMT_HEXINT 'x'
#define FMT_CHAR 'c'
#define FMT_PTR 'p' // note: a pointer can be treated as a long for x86_64
#define FMT_STR 's'
#define FMT_SHORT_PREFIX 'h'
#define FMT_LONG_PREFIX 'l'
#define FMT_LONG_LONG_PREFIX 'q'
// TODO: define format constants for the 3 bracket cases listed in instructions

typedef union{
   int int_val;
   long long_val;
   char *str_val;
} Args;

extern State serial_buffer;

__attribute__ ((format (printf, 1, 2)))
int printk(const char *fmt,...)
{
        if(!fmt) return ERR_NULL_PTR;
        va_list va;
        va_start(va, fmt);
        Args args;
        size_t len = strlen(fmt);
        int i = 0;
        while(i<len)
        {
                if(fmt[i] == FMT_SPEC)
                {
                        i++; // look at following char for the actual specifier
                        switch(fmt[i])
                        {
                                case FMT_MODULO:
                                        print_char(FMT_MODULO);
                                        break;
                                case FMT_CHAR:
                                        args.int_val = va_arg(va,int);
                                        print_char(args.int_val);
                                        break;
                                case FMT_PTR:
                                        args.long_val = va_arg(va,long);
                                        print_long_hex(args.long_val);
                                        break;
                                case FMT_STR:
                                        args.str_val = va_arg(va,char *);
                                        print_str(args.str_val);
                                        break;
                                case FMT_INT:
                                        args.int_val = va_arg(va,int);
                                        print_signed_long(args.int_val);
                                        break;
                                case FMT_UINT:
                                        args.int_val = va_arg(va,int);
                                        print_unsigned_long(args.int_val);
                                        break;
                                case FMT_HEXINT:
                                        args.int_val = va_arg(va,int);
                                        print_long_hex(args.int_val);
                                        break;
                                case FMT_SHORT_PREFIX: 
                                        i++;
                                        args.long_val = va_arg(va,int);
                                        switch(fmt[i])
                                        {
                                                case FMT_INT:
                                                        print_signed_long(
                                                                args.long_val);
                                                        break;
                                                case FMT_UINT:
                                                        print_unsigned_long(
                                                                args.long_val);
                                                        break;
                                                case FMT_HEXINT:
                                                        print_long_hex(
                                                                args.long_val);
                                                        break;
                                        }
                                        break;
                                case FMT_LONG_PREFIX: 
                                case FMT_LONG_LONG_PREFIX:
                                        i++;
                                        args.long_val = va_arg(va,long);
                                        switch(fmt[i])
                                        {
                                                case FMT_INT:
                                                        print_signed_long(
                                                                args.long_val);
                                                        break;
                                                case FMT_UINT:
                                                        print_unsigned_long(
                                                                args.long_val);
                                                        break;
                                                case FMT_HEXINT:
                                                        print_long_hex(
                                                                args.long_val);
                                                        break;
                                        }
                        }
                }
                // just a regular char, display it
                else print_char(fmt[i]);
                i++;
        }

        va_end(va);
        return 1;
}
void print_char(char c)
{
        VGA_display_char(c);   
        serial_write(c,&serial_buffer); 
}

void print_str(const char *str)
{
        //VGA_display_str(str);
        for(int i = 0; i < strlen(str); i++)
                print_char(str[i]);
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
        while(index--) print_char(digits[index]);
}

void print_signed_long(long l)
{
        unsigned char digits[ARR_SIZE];
        int index = 0;

        if(l<0)
        {
                print_char(MINUS_ASCII);
                if(l==LONG_MIN)
                {
                        digits[index++] = -1*(l%NEG_DEC_BASE) + NUM_ASCII_OFF;
                        l=l/DEC_BASE;
                }
                l=l*-1;
        }
        print_digits(digits,index,l);
}

void print_unsigned_long(unsigned long l)
{
        unsigned char digits[ARR_SIZE]; 
        print_digits(digits,0,l);
}

void print_digits(unsigned char digits[],int index,unsigned long l)
{
        unsigned char temp;
        if(l==0) digits[index++] = l+NUM_ASCII_OFF;
        while(l)
        {
                temp = (l%DEC_BASE) + NUM_ASCII_OFF;
                digits[index++] = temp;
                l=l/DEC_BASE;
        }
        while(index--) print_char(digits[index]);
}

void print_long_hex(unsigned long l)
{
        unsigned char digits[ARR_SIZE]; 
        int index = 0;
        unsigned char temp;
        print_str(HEX_PREFIX);
        if(l == 0)
        {
                print_digits(digits,0,l);
                return;
        }
        while(l)
        {
                temp = (l%HEX_BASE);
                if(temp >= 10)
                {
                        temp = (temp%10) + CHAR_ASCII_OFF;
                }
                else temp = temp + NUM_ASCII_OFF;
                digits[index++] = temp;
                l=l/HEX_BASE;
        }
        while(index--) print_char(digits[index]);
}
