/*
 * Nakul Nayak
 * Kernel Project
 * Description:
 * Prototypes for specific  string and memory 
 * manipulation functions from standard C library
*/

#include <stddef.h>
#include <stdint-gcc.h>

void *memset(void *dst, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
const char *strchr(const char *s, int c);
char *strdup(const char *s);
void outb(uint16_t, uint8_t);
uint8_t inb(uint16_t);
void io_wait(void);
void cli();
void sti();
/*void load_idtr();*/
