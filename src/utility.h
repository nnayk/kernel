/*
 * Nakul Nayak
 * Kernel Project
 * Description:
 * Prototypes for specific  string and memory 
 * manipulation functions from standard C library
*/

#include <stddef.h>
#include <stdint-gcc.h>
#ifndef _utility_h
#define _utility_h
/* Represents a path to a file */
typedef struct
{
        uint32_t depth; /* represents the number of levels away from root
                           the file is. Formulaically: depth = number of slashes + 1 */
        uint16_t **fnames; /* names of the files in the path */
}Path;

size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
const char *strchr(const char *s, int c);
char *strdup(const char *s);
void outb(uint16_t, uint8_t);
uint8_t inb(uint16_t);
uint16_t inw(uint16_t);
void io_wait(void);
void cli();
void sti();
void hlt();
void dbug_hlt(int);
char mapScanCodeToAscii(int);
void ltr(uint16_t);
int are_interrupts_enabled();
void *get_p4_addr();
void set_cr3(uint64_t);
void *get_cr2();
void bail();
int write_bitmap(uint8_t *,void *,uint16_t);
int are_buffers_equal(const void *, const void *,int);
int strcmp(const char *, const char *);
uint16_t *strsep(uint16_t **, const uint16_t *);
uint16_t *char_arr_to_uint16_arr(char *,size_t);
#endif
