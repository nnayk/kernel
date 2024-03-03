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
#include "mt_tests.h"
#include "snakes.h"

static int err;
State serial_buffer;
State kbd_buffer;
region ram[2];
region elf_region;
void *free_head;
ProcQueue *all_procs;
ProcQueue *ready_procs;
ProcQueue *kbd_blocked;
Process *curr_proc;
Process *next_proc;
Process main_proc;

void kmain()
{
        /*
        void *va = (void *)(0x8000000000);
        VA_t virt_addr = *((VA_t *)&va);
        printk("va=%p, p4 ind = %d\n",va,virt_addr.p4_index);
        va = (void *)(0x7FFFFFFFFFF);
        virt_addr = *((VA_t *)&va);
        printk("va=%p, p4 ind = %d\n",va,virt_addr.p4_index);
        */
        int loop = 1;
        while(!loop);

        main_proc.pid = 0;
        
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
        //display_pools();
        //kmalloc_tests();
        PROC_init();
        //simple_test();
        //numbers_test();
        sched_admit(ready_procs,&main_proc);
        PROC_run();
        setup_snakes(1);
        //display_threads(all_procs);
        //display_threads(ready_procs);
        //pf_simple_test();
        //pf_nonseq_test();
        //pf_stress_test();
        //while(!loop);
        //int *x=(void *)0x1000000000;
        //*x=2;
        //printk("x=%p\n",x);
        while(1)
        {
            PROC_run();
            if(ready_procs->proc_count == 1) display_threads(all_procs);
            hlt();
        }
}
