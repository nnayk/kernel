/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include "process.h"
#include "print.h"
#include "ps2.h"

void kbd_thread()
{
   while (1) printk("%c", KBD_read());
}
void kbd_tests()
{
    PROC_create_kthread(&kbd_thread,0);         
}
