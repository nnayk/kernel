#include "vga.h"
#include "print.h"
#include "limits.h"
#include "ps2.h"
#include "irq.h"
#include "utility.h"
#include "shared_buff.h"
#include "serial.h"

static int err;
State serial_buffer;

void kmain()
{
        printk("hi");
        int loop = 0;
        /*
        unsigned char data;
        int ptr[100];
        printk("%p\n",ptr);
        memset(ptr,100,100*sizeof(int));
        for(int i=0;i<100;i++) printk("%c",ptr[i]);
        char *x="aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa \
                aaaaaaaaaaaaaaaaaaaa";
        char *y="Hel\nl\ro";
        //unsigned char z = 255;
        char *z = "a8";
        VGA_display_str(z);
        //unsigned char z = 97;
        //while(!loop);
        printk("long min : %ld\n",LONG_MIN);
        printk("Here's a string: %s",x);
        printk("here's another string: %s",y);
        printk("char: %c, short_min = %hd\n",115,SHRT_MIN);
        printk("%hd\n",-9999);
        printk("Hello %s %d %p\n %x\n","hey",'a',x,20);
        printk("short ints: %hd %hu %hx\n",-9999,SHRT_MAX,SHRT_MAX);
        printk("long ints: %ld %ld %lu %lx\n",LONG_MAX,LONG_MIN,ULONG_MAX,
                        LONG_MIN);
        printk("long long ints: %qd %qd %qu %qx\n",LLONG_MAX,LLONG_MIN,
                        ULLONG_MAX,LLONG_MIN);
        printk("%hd\n",-32768);
        printk("here's a special symbol: %%. Oh and a number %d\n",59);
        printk("char: %c, short_min = %hd\n",115,SHRT_MIN);
        printk("char: %c, short_min = %hd\n",115,SHRT_MIN);
        printk("pointer: %p\n",&x);
        */
        ps2_init();
        kbd_init();
        serial_init();
        //for(int i=0;i<16;i++) printk("B4: irq %d: %d ",i,irq_get_mask(i));
        if((err=irq_init()) < 0)
                printk("irq_init failed w/error = %d\n",err);
        //for(int i=0;i<16;i++) printk("AFTER: irq %d: %d",i,irq_get_mask(i));
        /*
        while(!loop)
        {
                data = ps2_poll_read();
                //printk("scan code = %x\n",data);
                data = mapScanCodeToAscii(data);
                if(data) print_char(data);
        }
        */
        while(!loop);
}



