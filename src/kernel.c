#include "vga.h"
#include "print.h"
void kmain()
{
	int loop = 0;
        char *x="aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
        char *y="Hel\nl\ro";
        /*
        unsigned char z = 255;
        char z = "a";
        VGA_display_str(&z);
        unsigned char z = 97;
        */
        VGA_display_str(x);
        VGA_display_str(y);
        print_uchar(19);
        print_short(-32768);
        printk("Hello %s %d\n","hey",'a');
        print_long_decimal(-32768);
        print_long_hex(20);
        //VGA_clear();
	while(!loop)
	{

	}
}
