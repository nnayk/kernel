/*
 * Nakul Nayak
 * Kernel Project
 * Description:
 * Prototypes for specific  string and memory 
 * manipulation functions from standard C library
*/

void *memset2(void *dst, int c, size_t n);
void *memcpy2(void *dest, const void *src, size_t n);
size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
const char *strchr(const char *s, int c);
char *strdup(const char *s);
