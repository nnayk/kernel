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

void *memset(void *dst, int c, size_t n)
{
        if(n>LIMIT) return (void *)ERR_INPUT_SIZE;
        for(int i=0;i<n;i++)
        {
                if(!(dst+i)) return (void *)ERR_NULL_PTR;
                ((char *)dst)[i] = c;
        }

        return dst;
}
void *memcpy(void *dst, const void *src, size_t n)
{
        if(n>LIMIT) return (void *)ERR_INPUT_SIZE;
        for(int i=0;i<n;i++)
        {
                if(!(dst+i) || !(src+i)) return (void *)ERR_NULL_PTR;
                ((char *)dst)[i] = ((char *)src)[i];
        }
        return dst;
}

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

#if 0
int strcmp(const char *s1, const char *s2);
const char *strchr(const char *s, int c);
char *strdup(const char *s);
#endif
