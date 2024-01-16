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
#define LIMIT 5000

void *memset2(void *dst, int c, size_t n)
{
        if(n>LIMIT) return (void *)ERR_INPUT_SIZE;
        for(int i=0;i<n;i++)
        {
                if(!dst) return (void *)ERR_NULL_PTR;
                ((char *)dst)[i] = c;
        }

        return dst;
}
void *memcpy2(void *dst, const void *src, size_t n)
{
        if(n>LIMIT) return (void *)ERR_INPUT_SIZE;
        for(int i=0;i<n;i++)
        {
                if(!dst || !src) return (void *)ERR_NULL_PTR;
                ((char *)dst)[i] = ((char *)src)[i];
        }
        return dst;
}

size_t strlen(const char *s)
{
        if(!s) return (void *)ERR_NULL_PTR;
}
#if 0
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
const char *strchr(const char *s, int c);
char *strdup(const char *s);
#endif
