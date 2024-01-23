#include "vga.h"
#include "print.h"
#include "limits.h"
#include "ps2.h"

char mapScanCodeToAscii(int);

void kmain()
{
	//int loop = 0;
        //unsigned char data;
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
/*	
        while(!loop)
        {
                data = ps2_poll_read();
                printk("scan code = %x\n",data);
                //data = mapScanCodeToAscii(data);
                //if(data) printk("data = %c\n",data);
        }
*/
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

