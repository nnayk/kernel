/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include "constants.h"
#include "process.h"
#include "utility.h"
#include "memutils.h"
#include "scheduler.h"
#include "print.h"

#define STR(x) #x
#define XSTR(s) STR(s)

extern ProcQueue *all_procs;
extern ProcQueue *ready_procs;
extern Process *curr_proc;
extern Process *next_proc;
static int pid = 1;

void yield(void)
{
    asm volatile ("INT $" XSTR(YIELD_INT_NO));
}

void kexit(void)
{
    asm volatile ("INT $" XSTR(KEXIT_INT_NO));
    yield();
}

void PROC_init()
{
    all_procs = sched_init_queue();            
    ready_procs = sched_init_queue();            
}

void PROC_run(void)
{
        if(!ready_procs)
        {
            return;        
        }
        
        yield();
}

Process *PROC_create_kthread(kproc_t entry_pt, void* arg)
{
    Process *proc = kmalloc(sizeof(Process));
    proc->stack_start = alloc_kstack();          
    proc->rsp = (uint64_t)proc->stack_start - 32;
    proc->rdi = (uint64_t)arg;
    proc->rip = (uint64_t)entry_pt;
    proc->cs = KERNEL_CS;
    proc->rdi = (uint64_t)arg;
    proc->cr3 =  (uint64_t)get_p4_addr();
    proc->pid = pid++;

    sched_admit(all_procs,proc);
    sched_admit(ready_procs,proc);

    return proc;

}

void yield_isr(int int_num,int err_code,void *arg)
{
   reschedule();             
}

void kexit_isr(int int_num, int err_code,void *arg)
{
    if(!curr_proc)
    {
            printk("kexit_isr: curr_proc is null\n");
            bail();
    }

    /* free up the thread's resources */
    void *stack_low = curr_proc->stack_start - STACK_SIZE;
    kfree(stack_low);
    sched_remove(all_procs,curr_proc);
    sched_remove(ready_procs,curr_proc);
    kfree(curr_proc);
    curr_proc = NULL;
}

