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

void fat_init()
{
    if(DBUG) printk("inside fat init\n");
    parse_mbr();
    SuperBk *super = fat_probe();
    if(DBUG) printk("just so I don't get a warning/error: %p\n",super);
}
void parse_mbr()
{
    BD *dev = (BD *)ata_lst.devs[0];
    uint8_t *buffer = kmalloc(512*sizeof(uint8_t));
    ATABD_read_block(dev,0,(uint16_t *)buffer);
    for(int i=0;i<256;i++)
    {
        if(DBUG) printk("word %d = %hx\n",i,buffer[i]);        
    }
    //int loop=0;
    //while(!loop);
    //validate signatures
    if(DBUG) printk("buffer[255] = %hx\n",buffer[511]);
    if(((uint16_t *)buffer)[255] != 0xAA55)
    {
        if(DBUG) printk("parse_mbr: invalid mbr signature %hx!\n",buffer[255]);
        bail();
    }
    else if(DBUG) if(DBUG) printk("valid mbr signature\n");

    for(int i=0;i<4;i++)
    {
        memcpy(part_entries+i,buffer+446+i*16,sizeof(Partition_Entry));
        if(DBUG) printk("Partition entry %d contents:\n",i+1);
        display_partition_entry(part_entries[i]);
    }
}

void display_partition_entry(Partition_Entry entry)
{
    if(DBUG) printk("status = %d\n",entry.status);
    if(DBUG) printk("partition type = %x\n",entry.partition_type);
    if(DBUG) printk("first sector lba48 = %d\n",entry.first_sec_lba);
    if(DBUG) printk("number of sectors = %d\n",entry.num_secs);
}

SuperBk *fat_probe()
{
    if(DBUG) printk("inside fat_probe\n");
    SuperBk *super = kmalloc(sizeof(SuperBk));
    super->dev = ata_lst.devs[0];
    super->partition = part_entries;
    super->fat_hdr = kmalloc(sizeof(Fat_Hdr));

    /* read and validate Fat header */
    uint16_t *buffer = kmalloc(sizeof(uint16_t)*256);
    if(DBUG) printk("b4 read\n");
    ATABD_read_block((BD *)super->dev,super->partition->first_sec_lba,buffer);
    if(DBUG) printk("after read\n");
    memcpy(super->fat_hdr,buffer,sizeof(Fat_Hdr));
    if(super->fat_hdr->signature != 0x28 && super->fat_hdr->signature != 0x29)
    {
        printk("invalid fat32 signature: %hx\n",super->fat_hdr->signature);
        bail();
    }
    display_fat32(super->fat_hdr);
    return super;   
}

void display_fat32(Fat_Hdr *hdr)
{
    printk("Fat header attributes:\n");
    display_bpb(&hdr->bpb);
    if(hdr->bpb.num_sectors_per_fat == 0)
    {
        printk("real num sectors per fat = %d\n",hdr->sectors_per_fat);
    }

    printk("root dir cluster no. = %d\n",hdr->root_cluster_number);
    printk("name = %s\n",hdr->label);
}

void display_bpb(Fat_Bpb *bpb)
{
    printk("Fat BPB attributes:\n");
    printk("bytes per sector = %d\n",bpb->bytes_per_sector);
    printk("sectors per cluster = %d\n",bpb->sectors_per_cluster);
    printk("reserved sectors = %d\n",bpb->reserved_sectors);
    printk("num fats = %d\n",bpb->num_fats);
    printk("num dirents = %d\n",bpb->num_dirents);
    printk("total sectors = %d\n",bpb->tot_sectors);
    printk("num sectors per fat = %d\n",bpb->num_sectors_per_fat);
}
