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
SuperBk *super;

void fat_init()
{
    if(DBUG) printk("inside fat init\n");
    parse_mbr();
    SuperBk *super = fat_probe();
    if(DBUG) printk("just so I don't get a warning/error: %p\n",super);
    readdir(super->fat_hdr->root_cluster_number,0);
}
void parse_mbr()
{
    BD *dev = (BD *)ata_lst.devs[0];
    uint8_t *buffer = kmalloc(512*sizeof(uint8_t));
    ATABD_read_block(dev,0,(uint16_t *)buffer);
    /*
    for(int i=0;i<256;i++)
    {
        if(DBUG) printk("word %d = %hx\n",i,buffer[i]);        
    }
    */
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
    super = kmalloc(sizeof(SuperBk));
    super->dev = ata_lst.devs[0];
    super->partition_start = part_entries[0].first_sec_lba;
    super->fat_hdr = kmalloc(sizeof(Fat_Hdr));

    /* read and validate Fat header */
    uint16_t *buffer = kmalloc(sizeof(uint16_t)*256);
    if(DBUG) printk("b4 read\n");
    ATABD_read_block((BD *)super->dev,super->partition_start,buffer);
    if(DBUG) printk("after read\n");
    memcpy(super->fat_hdr,buffer,sizeof(Fat_Hdr));
    if(super->fat_hdr->bpb.bytes_per_sector != BLK_SIZE)
    {
        printk("block size is not 512\n");
        bail();
    }
    super->num_clusters = super->fat_hdr->bpb.tot_sectors * super->fat_hdr->bpb.sectors_per_cluster;
    super->num_secs_per_cluster = super->fat_hdr->bpb.sectors_per_cluster;
    super->fat_tbl = kmalloc(super->fat_hdr->sectors_per_fat*BLK_SIZE);
    uint32_t sector = 0;
    // cache fat table
    for(int i = 0;i<super->fat_hdr->sectors_per_fat;i++)
    {
        sector = super->partition_start+super->fat_hdr->bpb.reserved_sectors+i;
        ATABD_read_block((BD *)super->dev,sector,buffer);
        memcpy(super->fat_tbl+i*128,buffer,BLK_SIZE);
        //printk(" sector %d first 10 bytes:\n",sector);
        /*
        for(int j=0;j<10;j++)
        {
            printk("buffer: %hx\n",buffer[j]);
            printk("fat_tbl: %hx\n",super->fat_tbl[i*128+j] & 0x0FFFFFFF);
        }
        */
    }
    if(super->fat_hdr->signature != 0x28 && super->fat_hdr->signature != 0x29)
    {
        printk("invalid fat32 signature: %hx\n",super->fat_hdr->signature);
        bail();
    }
    display_fat32(super->fat_hdr);
    kfree(buffer);
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

void readdir(uint32_t cluster,int num_spaces)
{
    if(!valid_cluster(cluster))
    {
        printk("invalid cluster %d\n",cluster);
    }
     
    uint8_t *cbuffer = kmalloc(BLK_SIZE*super->num_secs_per_cluster);
    uint32_t sector;
    ATABD *dev = ata_lst.devs[0];
    Fat_Dir_Ent *dir_ent;
    int e1,e2;
    //Fat_LDir_Ent *ldir_ent;
    //TODO: fix while loop condition
    while(cluster && cluster < 0x0FFFFFF8)
    {
        printk("cluster=%hx\n",cluster);
        sector = cluster_to_sector(cluster);
        // read the cluster
        for(int cbuff_off=0;cbuff_off<super->num_secs_per_cluster;cbuff_off++)
        {
            ATABD_read_block((BD *)dev,sector,cbuffer+cbuff_off*BLK_SIZE);
        }
        // process the cluster and print its contents
        uint32_t dir_off = 0; // dir entry offset in current cluster
        int multiplier = 1; // number of entries to skip over (in the case of long entries)
        // get the first dir entry
        while(1)
        {
            multiplier = 1;
            dir_ent = (Fat_Dir_Ent *)(cbuffer+dir_off); // TODO: check this ptr arithmetic since cbuffer is of type uint32_t *
            // reached end of dir entries
            if(dir_ent->name[0] == 0) 
            {
                if(DBUG) printk("reached end of entries\n");
                break;
            }
            // skip, this entry has been deleted
            else if(dir_ent->name[0] == 0xE5) continue;
            for(int i=0;i<num_spaces;i++) print_char(' ');
            // recursively read the dir
            if(dir_ent->attr & FAT_ATTR_DIRECTORY)
            {
                printk("dir = %s\n",dir_ent->name);
                for(int w=0;w<strlen(dir_ent->name);w++)
                {
                    printk("index %d = %d\n",w,dir_ent->name[w]);
                }
                                
                e1 = dir_ent->name[0]=='.' && dir_ent->name[1]==' ';
                e2 = dir_ent->name[0]=='.' && dir_ent->name[1]=='.' && dir_ent->name[2]==' ';
                printk("e1=%d,e2=%d\n",e1,e2);
                printk("next cluster = %hx\n",dir_ent->cluster_hi | dir_ent->cluster_lo);
                if(!e1 && !e2) readdir(dir_ent->cluster_hi | dir_ent->cluster_lo,num_spaces+4);
            }
            // else if it's a file, just print the name
            else
            {
                // TODO: lfn check
                printk("%s\n",dir_ent->name);

            }

            dir_off += sizeof(Fat_Dir_Ent)*multiplier;
        }
        cluster = super->fat_tbl[cluster];
        // get the next cluster
    }
    kfree(cbuffer);
}

int valid_cluster(uint32_t cluster)
{
    return (cluster >= 2) && (cluster <= (super->num_clusters+2));
}

uint32_t cluster_to_sector(uint32_t cluster)
{
    return  super->partition_start + super->fat_hdr->bpb.reserved_sectors + super->fat_hdr->bpb.num_fats * super->fat_hdr->sectors_per_fat + cluster - 2;             
}
