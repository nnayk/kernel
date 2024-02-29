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
#include "process.h"
#include "scheduler.h"

static int err;
State serial_buffer;
region ram[2];
region elf_region;
void *free_head;
ProcQueue *all_procs;
ProcQueue *ready_procs;
Process *curr_proc;
Process *next_proc;
Process main_proc;  

void kmain()
{
#if 0
        int loop = 0;
        while(!loop);
#endif
        ram[0].start = INVALID_START_ADDR;
        elf_region.start = INVALID_START_ADDR;
        free_head = INVALID_START_ADDR;
        ps2_init();
        kbd_init();
        if((err=irq_init()) < 0)
                printk("irq_init failed w/error = %d\n",err);
        init_state(&serial_buffer);
        serial_init();
        mem_setup();
        setup_pt4();
        if(init_pools() < 0)
        {
                printk("init_pool error");
                return;
        }
        display_pools();
        //kmalloc_tests();
        PROC_init();
        //while(!loop);
        //int *x=(void *)0x1000000000;
        //*x=2;
        //printk("x=%p\n",x);
        while(1)
        {
            PROC_run();
            hlt();
        }
}
