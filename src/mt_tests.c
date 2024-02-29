/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include <stddef.h>
#include "print.h"
#include "process.h"
#include "scheduler.h"

void foo()
{
    printk("foo\n");
}
void simple_test()
{
    PROC_create_kthread(&foo,NULL);
}
