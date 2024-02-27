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
#include "tests.h"
#include "proc.h"

static int err;
State serial_buffer;
//region ram[0];
//region high_region;
region ram[2];
region elf_region;
void *free_head;
Context *allCtx;
Context *readyCtx;
Context *blockedCtx;
Context *currCtx;
Context *nextCtx;

void kmain()
{
        int loop = 0;
        ram[0].start = INVALID_START_ADDR;
        elf_region.start = INVALID_START_ADDR;
        free_head = INVALID_START_ADDR;
        ps2_init();
        kbd_init();
        if((err=irq_init()) < 0)
                printk("irq_init failed w/error = %d\n",err);
        init_state(&serial_buffer);
        serial_init();
        int y = 3;
        printk("%d\n",y);
        mem_setup();
        setup_pt4();
        if(init_pools() < 0)
        {
                printk("init_pool error");
                return;
        }
        display_pools();
        //pf_simple_test();
        //pf_nonseq_test();
        printk("VGA_display_str addr = %p\n",VGA_display_str);
        //pf_stress_test();
        kmalloc_tests();
        while(!loop);
        printk("a");
}
