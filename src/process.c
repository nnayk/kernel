/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include <stdint-gcc.h>
#include "constants.h"
#include "process.h"
#include "utility.h"

extern Process *all_proc;
extern Process *ready_proc;
extern Process *curr_proc;
extern Process *next_proc;

void PROC_run(void)
{
        if(!ready_proc)
        {
            return;        
        }
        yield();
}

Process *PROC_create_kthread(kproc_t entry_pt, void* arg)
{
    Process *proc = kmalloc(sizeof(Process));
    proc->stack_start = alloc_kstack();          
    proc->rsp = stack_start - 32;
    proc->rdi = (uint64_t)arg;
    proc->rip = kproc_t;
    proc->cs = KERNEL_CS;
    proc->rdi = arg;
    proc->cr3 =  get_p4_addr();

    return proc;

}

void PROC_reschedule(void)
{
            
}

static inline void PROC_yield(void);
{
    asm volatile ("INT $" XSTR(YIELD_INT_NO));
}
