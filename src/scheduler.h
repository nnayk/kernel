/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/
#include "process.h"


ProcQueue *sched_init_queue();
int sched_admit(ProcQueue *,Process *);
int sched_remove(ProcQueue *,Process *);
Process *reschedule();
int sched_proc_count(ProcQueue *);
void display_threads();
