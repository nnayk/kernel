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
#include "memutils.h"
#include "constants.h"
#include "print.h"

#define DBUG 1

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

uint8_t inw(uint16_t port)
{
        uint16_t ret;
        asm volatile ( "inw %1, %0"
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

void *get_p4_addr() {
    uint64_t cr3;
    asm volatile("mov %%cr3, %0" : "=r" (cr3));
    return (void *)cr3;
}

void set_cr3(uint64_t addr)
{
    asm volatile (
        "mov %0, %%cr3"   // Move the value to CR3
        :
        : "r" (addr)   // Input operand: cr3_value
    );        
}

void *get_cr2()
{
    uint64_t cr2;
    asm volatile("mov %%cr2, %0" : "=r" (cr2));
    return (void *)cr2;

}

void dbug_hlt(int dbug_flag)
{
        if(dbug_flag) hlt();
}

void bail()
{
    printk("BAILING!\n");
    //if(are_interrupts_enabled())
      //  cli();
    hlt();
}

/*
 * Description: Repeatedly fills a buffer starting from a given address with the bytesthat make up the address.
 * Params:
 * given_map -- OPTIONAL: buffer to fill
 * addr
 * size -- size of buffer to fill (must be a multiple of sizeof(void *) (i.e. 8))
 * Returns
 * status code
 */
int write_bitmap(uint8_t *given_map,void *addr,uint16_t size)
{
    if(size%sizeof(void *))
    {
            printk("write_bitmap: invalid size %d\n",size);
            return ERR_BAD_INPUT;
    }

    uint8_t bitmap[PAGE_SIZE]; // max bitmap size is PAGE_SIZE, only <size> 
                               //  bytes will be used though
    uint8_t *ptr = bitmap; // either <bitmap> or <given_map>
    if(given_map) ptr = given_map;
    /* populate bitmap */
    for(int i = 0;i<size;i+=sizeof(void *))
    {
        if((uint64_t)memcpy(ptr+i,&addr,sizeof(void *)) < 0)
        {
                printk("write_ptr: memcpy error\n");
                bail();
        }
    }
    /* write the bit pattern to the page */
    if((uint64_t)memcpy(addr,ptr,size) < 0)
    {
            printk("write_ptr: memcpy error\n");
            bail();
    }

    return SUCCESS; 
}

int are_buffers_equal(const void *ptr1, const void *ptr2,int size) {
    const unsigned char *byte_ptr1 = ptr1;
    const unsigned char *byte_ptr2 = ptr2;

    for (int i = 0; i < size; ++i) {
        if (byte_ptr1[i] != byte_ptr2[i]) {
            if(DBUG)
            {
                    printk("bytes %d differ: for ptr1 = %d,ptr2=%d\n",i,byte_ptr1[i],byte_ptr2[i]);
            }
            return 0; // Not equal
        }
    }
    return SUCCESS; // Equal
}

#if 0
int strcmp(const char *s1, const char *s2);
const char *strchr(const char *s, int c);
char *strdup(const char *s);
#endif
