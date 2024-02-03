/*
 * Nakul Nayak
 * Kernel Project
 * Description: Bulk of the ISR code for interrupts I handle.
*/

#include "print.h"
#include "irq.h"
#include "ps2.h"
#include "utility.h"

void kbd_isr(int int_num,int err_code,void *buffer)
{
        unsigned char data = ps2_poll_read();
        //data = mapScanCodeToAscii(data);
        printk("recv: %d\n",data);
        irq_end_of_interrupt(1);
}

