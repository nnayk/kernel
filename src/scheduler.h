/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/
#include "process.h"

typedef struct
{
        Process *head;
        Process *tail;
        int proc_count;
}ProcQueue;

ProcQueue *sched_init_queue();
int sched_admit(Process *);
int sched_remove(Process *);
Process *sched_next(ProcQueue *);
int sched_qlen(ProcQueue *);
int PROC_unblock_head(ProcQueue *); 
int PROC_unblock_all(ProcQueue *);
int PROC_block_on(ProcQueue *, int enable_ints); 
