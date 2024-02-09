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

int track_unused()
{
        printk("multiboot_start = %p\n",multiboot_start);
        // get size of the entire multiboot struct
        uint32_t total_size;
        uint32_t curr_off = 2 * sizeof(uint32_t); // current offset into multiboot struct
        tag_hdr header;
        uint8_t *addr;


        if((err=(uint64_t)memcpy(&total_size,multiboot_start,sizeof(uint32_t))) <= 0)
                return err;
        printk("total struct size = %d\n",total_size);

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
             memcpy(&header,addr,sizeof(tag_hdr));
             if(DBUG) printk("type = %d\n,size=%d\n",header.type,header.size);
             if(header.type==2)
             {
                     printk("bootloader name = %s\n",addr+8);
                      
             }
             curr_off += header.size;
        }

        return 1;
}

