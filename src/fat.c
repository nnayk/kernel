/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include <stdint-gcc.h>
#include "blockdev.h"
#include "print.h"
#include "memutils.h"
#include "process.h"
#include "utility.h"
#include "fat.h"

#define DBUG 1

Partition_Entry part_entries[4];
extern ATABD_dev_lst ata_lst;


void parse_mbr()
{
    BD *dev = (BD *)ata_lst.devs[0];
    uint8_t *buffer = kmalloc(512*sizeof(uint8_t));
    ATABD_read_block(dev,0,(uint16_t *)buffer);
    for(int i=0;i<256;i++)
    {
        printk("word %d = %hx\n",i,buffer[i]);        
    }
    //int loop=0;
    //while(!loop);
    //validate signatures
    printk("buffer[255] = %hx\n",buffer[511]);
    if(((uint16_t *)buffer)[255] != 0xAA55)
    {
        printk("parse_mbr: invalid mbr signature %hx!\n",buffer[255]);
        bail();
    }
    else if(DBUG) printk("valid mbr signature\n");

    for(int i=0;i<4;i++)
    {
        memcpy(part_entries+i,buffer+446+i*16,sizeof(Partition_Entry));
        printk("Partition entry %d contents:\n",i+1);
        display_partition_entry(part_entries[i]);
    }
}

void display_partition_entry(Partition_Entry entry)
{
    printk("status = %d\n",entry.status);
    printk("partition type = %d\n",entry.partition_type);
    printk("first sector lba48 = %d\n",entry.first_sec_lba);
    printk("number of sectors = %d\n",entry.num_secs);
}
