/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include <stddef.h>
#include "scheduler.h"
#include "memutils.h"
#include "utility.h"
#include "constants.h"
#include "error.h"
#include "print.h"

extern ProcQueue *all_procs;
extern ProcQueue *ready_procs;
extern Process *curr_proc;
extern Process *next_proc;
extern Process main_proc;

ProcQueue *sched_init_queue()
{
        ProcQueue *q = kmalloc(sizeof(ProcQueue));
        q->head = NULL;
        q->tail = NULL;
        q->proc_count = 0;
        return q;
}
int sched_admit(ProcQueue *q,Process *proc)
{
    if(!q)
    {
        printk("sched_admit: queue not set!\n");
        bail();
    }
    
    if(!q->head)
    {
            q->head = proc;
            q->tail = proc;
    }
    else
    {
            if(q == all_procs) q->tail->allNext = proc;
            if(q == ready_procs) q->tail->next = proc;
            q->tail = proc;
    }

    proc->next = NULL;
    q->proc_count++;
    return SUCCESS;
}

int sched_remove(ProcQueue *q, Process *victim)
{
        Process *prev; /* saves Process that points to victim */
        if(!victim || !q->head) 
        {
                printk("sched_remove: bad args: q = %p, victim = %p\n",q,victim);
                bail();
        }

        /* remove head Process from queue */
        if(victim == q->head)
        {
               if(q==all_procs) q->head = victim->allNext;
               else q->head = victim->next;
               if(victim == q->tail)
               {
                       q->tail = q->head;
               }
               q->proc_count--;
               return SUCCESS;
        }

        prev = q->head;

        if(q==all_procs)
        {
                /* iterate till victim is reached */
                while(prev && prev->allNext != victim)
                {
                        prev = prev->allNext;
                }
        }
        
        else
        {
                /* iterate till victim is reached */
                while(prev && prev->next != victim)
                {
                        prev = prev->next;
                }
        }

        if(!prev)
        {
                printk("sched_remove: invalid victim pid = %ld, ready count = %d\n",victim->pid,ready_procs->proc_count);
                bail();
        }

        if(victim == q->tail)
        {
                q->tail = prev;
        }

        /* remove prev's pointer to victim */
        if(q==all_procs)
        {
                prev->allNext = victim->allNext;
                victim->allNext = NULL;
        }
        /* remove prev's pointer to victim */
        else
        {
                prev->next = victim->next;
                victim->next = NULL;
        }
        q->proc_count--;

        return SUCCESS;
}

#if 0
int PROC_unblock_head(ProcQueue *); 
int PROC_unblock_all(ProcQueue *);
int PROC_block_on(ProcQueue *, int enable_ints); 
#endif

Process *reschedule()
{
        // if only main thread is on the queue then return
        if(ready_procs->proc_count == 1)
        {
                if(ready_procs->head != &main_proc)
                {
                        printk("reschedule: ERROR only ready proc and it's not the main proc!\n");
                        bail();

                }
        }
        
        Process *temp = ready_procs->head;
        // don't run the main thread if there are other threads to run
        if((temp == &main_proc) && (ready_procs->proc_count > 1)) 
        {
                sched_remove(ready_procs,temp);
                sched_admit(ready_procs,temp);
                temp = ready_procs->head;
        }
        sched_remove(ready_procs,temp);
        sched_admit(ready_procs,temp);
        next_proc = temp;
        return next_proc;
}

int sched_proc_count(ProcQueue *q)
{
        return q->proc_count;
}

void display_threads(ProcQueue *q)
{
        Process *proc = q->head;
        if(q->proc_count == 0)
        {
                printk("no procs on the given queue\n");
                return;
        }
        for(int i=0;i<q->proc_count;i++)
        {
            printk("thread %d id = %ld\n",i,proc->pid);
            if(q==ready_procs) proc = proc->next;
            else if(q==all_procs) proc = proc->allNext;
        }

        if(proc)
        {
            printk("uhoh\n");
            bail();
        }
}

