/*
 * Nakul Nayak
 * Kernel Project
 * Description:
 * Implementations of specific string
 * and memory manipulation functions
 * from libc
*/

#include "error.h"
#include "utility.h"


size_t strlen(const char *s)
{
        size_t len = 0;
        while(*s) 
        {
                len++;
                s++;
        }
        return len;
}

char *strcpy(char *dest, const char *src)
{
        //if(!src) return (void *)ERR_NULL_PTR;
        size_t len = strlen(src); 
        for(size_t i=0;i<=len;i++) // account for trailing null byte
        {
                if(!(dest+i) || !(src+i)) return (void *)ERR_NULL_PTR;
                dest[i] = src[i];
        }
        return dest;
}

void outb(uint16_t port, uint8_t val)
{
        asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

uint8_t inb(uint16_t port)
{
        uint8_t ret;
        asm volatile ( "inb %1, %0"
        : "=a"(ret)
        : "Nd"(port) );
        return ret;
}

void io_wait(void)
{
        outb(0x80,0);
}

void cli() {
        asm volatile ("cli");
}

void sti() {
        asm volatile ("sti");
}

void hlt() {
        asm volatile ("hlt");
}

void ltr(uint16_t selector) {
        asm volatile ("ltr %0" : :"r"(selector));
}

int are_interrupts_enabled()
{
    unsigned long flags;
    asm volatile ( "pushf\n\t"
                   "pop %0"
                   : "=g"(flags) );
    return flags & (1 << 9);
}

// Function to map scan codes to ASCII characters
char mapScanCodeToAscii(int scanCode) {
    // Define a lookup table
    const char scanCodeToAscii[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
        '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',
        ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
        '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
        0, '*', 0, ' ', 0
    };

    // Check if the scan code is within the bounds of the lookup table
    if (scanCode >= 0 && scanCode < 
                    sizeof(scanCodeToAscii) / sizeof(scanCodeToAscii[0])) {
        return scanCodeToAscii[scanCode];
    }

    // Return 0 for undefined scan codes
    return 0;
}

#if 0
int strcmp(const char *s1, const char *s2);
const char *strchr(const char *s, int c);
char *strdup(const char *s);
#endif
