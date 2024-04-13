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
#include "blockdev.h"
#include "fat.h"

static int err;
State serial_buffer;
State kbd_buffer;
region ram[2];
region elf_region;
void *free_head;
ProcQueue *all_procs;
ProcQueue *ready_procs;
ProcQueue *kbd_blocked;
ProcQueue *ata_blocked;
Process *curr_proc;
Process *next_proc;
Process main_proc;

void kmain()
{
        // init resources
        main_proc.pid = 0;
        ram[0].start = INVALID_START_ADDR;
        elf_region.start = INVALID_START_ADDR;
        free_head = INVALID_START_ADDR;
        ps2_init();
        kbd_init();
        if((err=irq_init()) < 0)
                printk("irq_init failed w/error = %d\n",err);
        init_state(&serial_buffer);
        init_state(&kbd_buffer);
        serial_init();
        mem_setup();
        setup_pt4();
        if(init_pools() < 0)
        {
                printk("init_pool error");
                return;
        }
        PROC_init();
        sched_admit(ready_procs,&main_proc);
        PROC_run();
        setup_ata();
        PROC_create_kthread((kproc_t)fat_init,NULL);
        // run threads
        while(1)
        {
            
            PROC_run();
            hlt();
        }
}
