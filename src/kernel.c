#include "vga.h"
#include "print.h"
#include "limits.h"
#include "ps2.h"
#include "irq.h"
#include "utility.h"
#include "memutils.h"
#include "serial.h"
#include "shared_buff.h"

static int err;
State serial_buffer;
region *unused_head;
region *free_head;


void kmain()
{
        int loop = 0;
        ps2_init();
        kbd_init();
        if((err=irq_init()) < 0)
                printk("irq_init failed w/error = %d\n",err);
        init_state(&serial_buffer);
        serial_init();
        /*
        while(!loop)
        {
                data = ps2_poll_read();
                //printk("scan code = %x\n",data);
                data = mapScanCodeToAscii(data);
                if(data) print_char(data);
        }
        */
        track_unused();
        while(!loop);
        printk("a");
}



