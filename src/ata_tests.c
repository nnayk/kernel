/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include <stdint-gcc.h>
#include "blockdev.h"
#include "process.h"
#include "tests.h"
#include "memutils.h"
#include "print.h"
#include "scheduler.h"

extern ATABD_dev_lst ata_lst;
extern ProcQueue *ready_procs;
static void read_block_32(void *arg)
{
    printk("entered read_block_32\n");
    ATABD *dev = ata_lst.devs[0];
    uint16_t *buffer = kmalloc(sizeof(uint16_t)*256);
    ATABD_read_block((BD *)dev,0,buffer);
    for(int i=0;i<256;i++)
    {
        printk("word %d = %hx\n",i,buffer[i]);        
    }
}

static void simple_test()
{
    PROC_create_kthread((kproc_t)read_block_32,0);
}
int ata_tests()
{
        simple_test();
        return 1;
}
