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
#include "paging.h"

#define STR(x) #x
#define XSTR(s) STR(s)
#define DBUG 0

extern ProcQueue *all_procs;
extern ProcQueue *ready_procs;
extern Process *curr_proc;
extern Process *next_proc;
extern Process main_proc;
static int pid = 1;

void yield(void)
{
    asm volatile ("INT $" XSTR(YIELD_INT_NO));
    curr_proc = next_proc;
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
    curr_proc = NULL;
    next_proc = NULL;
}

void PROC_run(void)
{
        if(!ready_procs)
        {
            return;        
        }
        
        curr_proc = &main_proc;
        next_proc = curr_proc;
        sched_admit(ready_procs,curr_proc);
        // save current (caller's context) in order to return to kmain after 
        // yield call
        yield();
        sched_admit(ready_procs,&main_proc);
        if(DBUG) printk("returned from yield\n");
}

Process *PROC_create_kthread(kproc_t entry_pt, void* arg)
{
    Process *proc = kmalloc(sizeof(Process));
    proc->stack_start = alloc_kstack();          
    proc->rsp = (uint64_t)proc->stack_start - 32;
    proc->rip = (uint64_t)PROC_wrapper;
    proc->cs = KERNEL_CS;
    proc->rdi = (uint64_t)entry_pt;
    proc->rsi = (uint64_t)arg;
    proc->cr3 = (uint64_t)get_p4_addr();
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
    if(!curr_proc || curr_proc == &main_proc)
    {
            printk("kexit_isr: curr_proc is invalid\n");
            bail();
    }

    /* free up the thread's resources */
    void *stack_start = curr_proc->stack_start;
    //kfree(stack_low);
    kstack_free_pages(stack_start,STACK_PAGES);
    sched_remove(all_procs,curr_proc);
    sched_remove(ready_procs,curr_proc);
    kfree(curr_proc);
}

void PROC_wrapper(kproc_t func, void * arg)
{
    (func)(arg);
    kexit();
}
