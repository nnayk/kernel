/*
 * Nakul Nayak
 * Kernel Project
 * Description: 
 * Print function prototypes
*/

int printk(const char *,...)__attribute__ ((format (printf, 1, 2)));
void print_char(char);
void print_str(const char *);
void print_uchar(unsigned char);
void print_short(short);
void print_long_hex(unsigned long);
void print_signed_long(long);
void print_unsigned_long(unsigned long l);
void print_digits(unsigned char[],int,unsigned long);
