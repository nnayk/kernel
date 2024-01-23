#include "vga.h"
#include "print.h"
#include "limits.h"
#include "ps2.h"

void kmain()
{
	int loop = 0;
        unsigned char data;
        /*
        char *x="aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa \
                aaaaaaaaaaaaaaaaaaaa";
        char *y="Hel\nl\ro";
        *
        unsigned char z = 255;
        char z = "a";
        VGA_display_str(&z);
        unsigned char z = 97;
        *
        //while(!loop);
        printk("long min : %ld\n",LONG_MIN);
        printk("Here's a string: %s",x);
        printk("here's another string: %s",y);
        printk("char: %c, short_min = %hd\n",115,SHRT_MIN);
        printk("Hello %s %d %p\n %x\n","hey",'a',x,20);
        printk("short ints: %hd %hu %hx\n",-99999,SHRT_MAX,SHRT_MAX);
        printk("long ints: %ld %ld %lu %lx\n",LONG_MAX,LONG_MIN,ULONG_MAX,
                        LONG_MIN);
        printk("long long ints: %qd %qd %qu %qx\n",LLONG_MAX,LLONG_MIN,
                        ULLONG_MAX,LLONG_MIN);
        printk("%hd\n",-32768);
        printk("here's a special symbol: %%. Oh and a number %d\n",59);
        printk("Here's a string: %s",x);
        printk("here's another string: %s",y);
        printk("char: %c, short_min = %hd\n",115,SHRT_MIN);
        printk("Here's a string: %s",x);
        printk("here's another string: %s",y);
        printk("char: %c, short_min = %hd\n",115,SHRT_MIN);
        printk("why hello there.... %s\r%p\n","nope",&x);
        //VGA_clear();
        */
        ps2_init();
        kbd_init();
	while(!loop)
        {
                data = ps2_poll_read();
                printk("data = %x\n",data);
        }
}

