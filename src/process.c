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
#include "error.h"

#define STR(x) #x
#define XSTR(s) STR(s)
#define DBUG 0

extern ProcQueue *all_procs;
extern ProcQueue *ready_procs;
extern ProcQueue *kbd_blocked;
extern Process *curr_proc;
extern Process *next_proc;
extern Process main_proc;
static int pid = 1;

void yield(void)
{
    //printk("yielding to next thread\n");
    asm volatile ("INT $" XSTR(YIELD_INT_NO));
    curr_proc = next_proc;
}

void kexit(void)
{
    asm volatile ("INT $" XSTR(KEXIT_INT_NO));
    kfree(curr_proc);
    curr_proc = NULL;
    yield();
}

void PROC_init()
{
    all_procs = sched_init_queue();            
    ready_procs = sched_init_queue();
    kbd_blocked = sched_init_queue();
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
        // save current (caller's context) in order to return to kmain after 
        // yield call
        yield();
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
}

void PROC_wrapper(kproc_t func, void * arg)
{
    (func)(arg);
    kexit();
}
int PROC_block_on(ProcQueue *q, int enable_ints)
{
        if (!q)
        {
                printk("PROC_block_on: null queue input\n");
                bail();
        }
        sched_remove(ready_procs,curr_proc);
        sched_admit(q,curr_proc);
        if (enable_ints) sti();
        yield();
        return SUCCESS;
}

int PROC_unblock_head(ProcQueue *q)
{
    if(!q || !q->head) 
    {
            printk("PROC_unblock_head: bad arg\n");
            bail();
    }
    Process *head = q->head;
    sched_remove(q,q->head);
    sched_admit(ready_procs,head);
    return SUCCESS;
}
int PROC_unblock_all(ProcQueue *q)
{
        if (!q)
        {
                printk("PROC_unblock_all: null queue input\n");
                bail();
        }

        for(int i=0;i<q->proc_count;i++) PROC_unblock_head(q);

        return SUCCESS;
}
