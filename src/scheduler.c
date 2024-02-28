/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

ProcQueue *sched_init_queue()
{
        ProcQueue *q = kmalloc(sizeof(ProcQueue *));
        q->head = NULL;
        q->tail = NULL;
        q->proc_count = 0;
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

    q->qlen++;
}

int sched_remove(ProcQueue *q, Process *victim)
{
        Process prev; /* saves Process that points to victim */
        if(!victim || !q->head) 
        {
                printk("sched_remove: bad args\n");
                bail();
        }

        /* remove head Process from q->eue */
        if(victim == q->head)
        {
               q->head = victim->next;
               if(victim == q->tail)
               {
                       q->tail = NULL;
               }
               q->qlen--;
               return;
        }

        prev = q->head;

        /* iterate till victim is reached */
        while(prev && prev->next != victim)
        {
                prev = prev->next;
        }

        if(!prev)
        {
                printf("sched_remove: invalid victim\n");
                bail();
        }

        if(victim == q->tail)
        {
                q->tail = prev;
        }

        /* remove prev's pointer to victim */
        prev->next = victim->next;
        q->qlen--;
}

#if 0
int PROC_unblock_head(ProcQueue *); 
int PROC_unblock_all(ProcQueue *);
int PROC_block_on(ProcQueue *, int enable_ints); 
#endif

Process *reschedule()
{
        if(!curr_proc)
        {
                printk("reschedule: no processes to schedule\n");
                return NULL;
        }
        if(!ready_procs)
        {
                printk("sched_next: ready procs queue not set\n");
                return NULL;
        }
        
        Process *temp = ready_procs->head;
        sched_remove(ready_procs,temp);
        sched_admit(ready_procs,temp);
        next_proc = ready_procs->head;
        return next_proc;
}

int sched_qlen(ProcQueue *q)
{
        return q->qlen;
}
