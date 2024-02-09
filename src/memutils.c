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
#define DBUG 1

extern region *unused_head;
extern region *free_head;
extern uint8_t *multiboot_start;
static int err;

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
                     if((err=(uint64_t)memcpy(memhdr,addr,sizeof(memtag_hdr_t))) < 0)
                             return err;
             }
             else if(temp.type == ELF_TAG)
             {
                     if((err=(uint64_t)memcpy(elfhdr,addr,sizeof(elftag_hdr_t))) < 0)
                             return err;
             }

             curr_off += temp.size;
        }

        return SUCCESS;
}

int setup_unused(memtag_hdr_t mmap_data,elftag_hdr_t elf_data)
{
        printk("inside setup_unused\n");
        if(DBUG) printk("mmap info entry size (should be 24)  = %d\n",mmap_data.entry_size);
        return SUCCESS;
}

