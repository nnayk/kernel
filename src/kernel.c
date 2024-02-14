#include "vga.h"
#include "print.h"
#include "limits.h"
#include "ps2.h"
#include "irq.h"
#include "utility.h"
#include "memutils.h"
#include "serial.h"
#include "shared_buff.h"
#include "paging.h"

static int err;
State serial_buffer;
region low_region;
region high_region;
region elf_region;
void *free_head;


void kmain()
{
        int loop = 0;
        low_region.start = INVALID_START_ADDR;
        elf_region.start = INVALID_START_ADDR;
        free_head = INVALID_START_ADDR;
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
        mem_setup();
        pf_alloc(); // ignore 0x0
        //pf_simple_test();
        //pf_nonseq_test();
        printk("VGA_display_str addr = %p\n",VGA_display_str);
        PTE_t temp;
        temp.present=1;
        allocate_pte(&temp);
        //pf_stress_test();
        while(!loop);
        printk("a");
}
