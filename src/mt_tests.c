/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include <stddef.h>
#include "print.h"
#include "process.h"
#include "scheduler.h"

static void indentnum(void *);

void foo()
{
    printk("foo\n");
}

void numbers_test()
{
  long i;
  int numThreads = 10;

  /* spawn a number of individual LWPs */
  for(i=1;i<=numThreads;i++) {
    PROC_create_kthread((kproc_t)indentnum,(void*)i);
  }
}

static void indentnum(void *num) {
  /* print the number num num times, indented by 5*num spaces
   * Not terribly interesting, but it is instructive.
   */
  long i;
  int howfar;
  howfar=(long)num;              /* interpret num as an integer */
  for(i=1;i<=howfar;i++){
    printk("%d\n",howfar);
    yield();                /* let another have a turn */
  }
  
  kexit();                /* bail when done.  This should
                                 * be unnecessary if the stack has
                                 * been properly prepared
                                 */
}

void simple_test()
{
    PROC_create_kthread(&foo,NULL);
}
