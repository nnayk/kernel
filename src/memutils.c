/*
 * Nakul Nayak
 * Kernel Project
 * Description: Memory manipulation/access functions
*/

#include <stddef.h>
#include <stdint-gcc.h>
#include "error.h"
#include "memutils.h"
#include "print.h"

#define LIMIT 5000
#define MMAP_TAG 6
#define ELF_TAG 9
#define FREE_RAM_TYPE 1
#define MEM_INFO_OFF 16
#define SEC_INFO_OFF 20
#define DBUG 1

//extern region unused_head;
//extern region free_head;
extern region low_region;
extern region high_region;
extern void *free_head;
extern uint8_t *multiboot_start;
static int err;
static uint32_t num_frames; // total number of physical frames

void *memset(void *dst, int c, size_t n)
{
        if(n>LIMIT) return (void *)ERR_INPUT_SIZE;
        for(int i=0;i<n;i++)
        {
                //if(!(dst+i)) return (void *)ERR_NULL_PTR;
                ((char *)dst)[i] = c;
        }

        return dst;
}
void *memcpy(void *dst, const void *src, size_t n)
{
        if(n>LIMIT) return (void *)ERR_INPUT_SIZE;
        for(int i=0;i<n;i++)
        {
                if(!(dst+i) || !(src+i)) return (void *)ERR_NULL_PTR;
                ((char *)dst)[i] = ((char *)src)[i];
        }
        return dst;
}

int mem_setup()
{
        memtag_hdr_t mmap_data; // save tag 6 
        elftag_hdr_t elf_data;  // save tag 9
        if((err = track_unused(&mmap_data,&elf_data)) < 0)
                return err;
        if((err = setup_unused(mmap_data,elf_data)) < 0)
        {
                printk("error = %d\n",err);
                return err;
        }
        return SUCCESS;
}

int track_unused(memtag_hdr_t *memhdr,elftag_hdr_t *elfhdr)
{
        printk("multiboot_start = %p\n",multiboot_start);
        // get size of the entire multiboot struct
        uint32_t total_size;
        uint32_t curr_off = 2 * sizeof(uint32_t); // current offset into multiboot struct
        tag_hdr temp;
        uint8_t *addr;

        if((err=(uint64_t)memcpy(&total_size,multiboot_start,sizeof(uint32_t))) <= 0)
                return err;
        printk("total struct size = %d\n",total_size);
        int loop=1;
        while(!loop);
        // loop until tag type = 8 reached
        while(curr_off < total_size)
        {
             addr = multiboot_start+curr_off;
             if(((uint64_t)addr)%8)
             {
                     printk("not an even multiple!\n");
                     curr_off += (8-(uint64_t)addr%8);
                     addr = multiboot_start+curr_off;
             }
             memcpy(&temp,addr,sizeof(tag_hdr));
             if(DBUG) printk("type = %d\n,size=%d\n",temp.type,temp.size);
             if(temp.type == MMAP_TAG)
             {
                     printk("addr arrays = %p\n",addr+16);
                     if((err=(uint64_t)memcpy(memhdr,addr,sizeof(memtag_hdr_t))) < 0)
                             return err;
                     memhdr->entry_start = addr+MEM_INFO_OFF;
                     printk("addr arrays in struct = %p\n",memhdr->entry_start);
             }
             else if(temp.type == ELF_TAG)
             {
                     if((err=(uint64_t)memcpy(elfhdr,addr,sizeof(elftag_hdr_t))) < 0)
                             return err;
                     elfhdr->entry_start = addr+SEC_INFO_OFF;
             }
             else if(temp.type == 4)
        {
                printk("low mem size = %d, high mem size = %d\n",(uint32_t)*(addr+8),(uint32_t)*(addr+12));
        }

             curr_off += temp.size;
        }

        return SUCCESS;
}

int setup_unused(memtag_hdr_t mmaphdr,elftag_hdr_t elfhdr)
{
        int num_mem_entries = (mmaphdr.size)/mmaphdr.entry_size;
        int num_elf_entries = elfhdr.entry_count;
        uint8_t *mem_addr = mmaphdr.entry_start;
        uint8_t *elf_addr = elfhdr.entry_start;
        uint8_t low_region_set = 0; // need to use this var. b/c low region starts at 0x0
        memtag_entry_t mementry;
        elf_entry_t elfentry;
        printk("inside setup_unused\n");
        printk("addr arrays in struct = %p,version=%d\n",mmaphdr.entry_start,mmaphdr.entry_version);
        if(DBUG) 
        {
                printk("mmap info entry size (should be 24)  = %d, num_mem_entries = %d\n",mmaphdr.entry_size,num_mem_entries);
                printk("number of section entries = %d\n",num_elf_entries);
        }
#if 1        
        for(int i=0;i<num_mem_entries;i++)
        {
                memcpy(&mementry,mem_addr,mmaphdr.entry_size);
                if(DBUG) printk("entry %d: start_addr = %ld,size=%ld,type=%d\n",i,(uint64_t)mementry.start_addr,mementry.size,mementry.type);
                if(mementry.type == FREE_RAM_TYPE)
                {
                        if(!low_region_set)
                        {
                                low_region_set = 1;
                                low_region.start = mementry.start_addr;
                                low_region.curr = low_region.start;
                                low_region.end = mementry.start_addr+mementry.size; //exclusive end address
                                num_frames = mementry.size/PAGE_SIZE;
                                if(DBUG) printk("Number of frames in low = %d\n",num_frames);
                        }
                        else
                        {
                                high_region.start = mementry.start_addr;
                                high_region.curr = high_region.start;
                                high_region.end = mementry.start_addr+mementry.size;
                                low_region.next = high_region.start;
                                num_frames += mementry.size/PAGE_SIZE;
                                if(DBUG) printk("number of frames in total = %d\n",num_frames);
                        }
                }
                mem_addr += mmaphdr.entry_size;
        }
        for(int i=0;i<num_elf_entries;i++)
        {
                memcpy(&elfentry,elf_addr,elfhdr.entry_size);
                if(DBUG) printk("entry %d: start_addr = %ld,size=%ld,type=%d\n",i,(uint64_t)elfentry.seg_addr,elfentry.seg_size,elfentry.type);
                if(elfentry.seg_size)
                {
                    printk("Found the kernel code!\n");
                    if(DBUG)
                    {
                            // check no low region overlap
                            if(low_region.start < elfentry.seg_addr && low_region.start < elfentry.seg_addr)
                                    printk("No overlap for low region\n");
                            // check no high region overlap
                            if(high_region.start < elfentry.seg_addr && high_region.start < elfentry.seg_addr)
                                    printk("No overlap for high region\n");
                    }
                }
                elf_addr += elfhdr.entry_size;
        }
#endif
        return SUCCESS;
}

void *pf_alloc()
{
        void *pg_start = NULL;
        // check unused regions list
        if((low_region.curr + PAGE_SIZE < low_region.end))
        {
            pg_start = low_region.curr;
            low_region.curr += PAGE_SIZE;
        }
        else if((high_region.curr + PAGE_SIZE < high_region.end))
        {
            pg_start = high_region.curr;
            high_region.curr += PAGE_SIZE;
        }
        // check free list
        else if(free_head)
        {
                pg_start = free_head;
                /* TODO: properly update free head to point to next free frame */
        }
        return pg_start;
}

void pf_alloc_simple_test()
{
        
        return;
}
