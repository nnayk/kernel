/*
 * Nakul Nayak
 * Kernel Project
 * Description: Memory manipulation/access functions
*/

#include <stdint-gcc.h>
#include "error.h"
#include "memutils.h"
#include "print.h"
#include "constants.h"
#include "utility.h"
#include "paging.h"

#define LIMIT 5000
#define MMAP_TAG 6
#define ELF_TAG 9
#define FREE_RAM_TYPE 1
#define MEM_INFO_OFF 16
#define SEC_INFO_OFF 20
#define DBUG 0

uint64_t kheap = KHEAP_START; //points to top of kernel heap
uint64_t kstack = KSTACK_START; //points to topmost of kernel stack (equivalent to rsp for the most recently allocated kernel stack)
//extern region unused_head;
//extern region free_head;
extern region low_region;
extern region elf_region;
extern region high_region;
extern region ram[];
extern void *free_head;
extern uint8_t *multiboot_start;
static uint64_t err;
static uint32_t num_frames_total; // total number of physical frames
static uint32_t num_frames_low; // number of physical frames in low region
static uint32_t num_frames_high; // number of physical frames in high region
static KmallocPool ram_pools[NUM_POOLS];

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
                printk("mem_setup: error = %ld\n",err);
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
             if(DBUG) printk("type = %d\n,size=%hx\n",temp.type,temp.size);
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
                     printk("entry 1: type = %d, seg. addr = %p\n",*(uint32_t *)(addr+24),addr+36);
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
                if(DBUG) printk("mem entry %d: start_addr = %p,size=%ld,type=%d\n",i,mementry.start_addr,mementry.size,mementry.type);
                if(mementry.type == FREE_RAM_TYPE)
                {
                        if(!low_region_set)
                        {
                                low_region_set = 1;
                                low_region.start = mementry.start_addr;
                                low_region.curr = low_region.start;
                                low_region.end = mementry.start_addr+mementry.size; //exclusive end address
                                num_frames_low = mementry.size/PAGE_SIZE;
                                num_frames_total = num_frames_low;
                                if(DBUG) printk("Number of frames in low = %d\n",num_frames_total);
                        }
                        else
                        {
                                high_region.start = mementry.start_addr;
                                high_region.curr = high_region.start;
                                high_region.end = mementry.start_addr+mementry.size;
                                low_region.next = &high_region;
                                num_frames_high = mementry.size/PAGE_SIZE;
                                num_frames_total += num_frames_high;
                                if(DBUG) printk("number of frames in total = %d\n",num_frames_total);
                        }
                }
                mem_addr += mmaphdr.entry_size;
        }
        for(int i=0;i<num_elf_entries;i++)
        {
                memcpy(&elfentry,elf_addr,elfhdr.entry_size);
                if(DBUG) printk("elf entry %d: start_addr = %p,size=%ld,type=%d\n",i,elfentry.seg_addr,elfentry.seg_size,elfentry.type);
                if(elfentry.seg_size)
                {
                    // mark the start of the kernel code region
                    if(elf_region.start == INVALID_START_ADDR)
                            elf_region.start = elfentry.seg_addr;
                    printk("Found the kernel code!\n");
                    if(DBUG)
                    {
                            // check no high region overlap
                            if(high_region.start < elfentry.seg_addr && high_region.end < elfentry.seg_addr)
                                    printk("No overlap for high region\n");
                    }
                }
                elf_addr += elfhdr.entry_size;
        }
        
        elf_region.end = elfentry.seg_addr+elfentry.seg_size;
#endif
        // handle the memory and elf overlap
        // check no low region overlap
        if(low_region.start < elf_region.start && low_region.start < elf_region.end)
        {
                printk("No low region overlap!\n");
        }
        else
        {
                printk("There IS low region overlap!\n");
        }
        // check no high region overlap
        if(high_region.start < elf_region.start && high_region.end < elf_region.end)
        {
                printk("No high region overlap!\n");
        }
        else
        {
                if(DBUG) printk("There IS high region overlap!\n");
                high_region.start = page_align_up(elf_region.end);
                high_region.curr = high_region.start;
                num_frames_total -= num_frames_high;
                num_frames_high = (high_region.end-high_region.start)/PAGE_SIZE;
                num_frames_total += num_frames_high;
                if(DBUG) printk("new num_frames_high = %d, num_frames_total = %d\n",num_frames_high,num_frames_total);
                
        }
        high_region.next = INVALID_START_ADDR;
        if(DBUG)
        {
            region *temp = &low_region;
            if(DBUG) printk("temp.start = %p\n",temp->start);
            while(temp != INVALID_START_ADDR)
            {
                if(DBUG) printk("start = %p, end = %p\n",temp->start,temp->end);
                temp = temp->next;
            }
            if(DBUG) printk("elf start = %p, end = %p\n",elf_region.start,elf_region.end);
        }

        return SUCCESS;
}

// allocates an entire physical frame
void *pf_alloc()
{
        void *pg_start = NULL;
        // check free list
        if(free_head != INVALID_START_ADDR)
        {
                pg_start = free_head;
                memcpy(&free_head,pg_start,sizeof(void *));
        }
        // check unused regions list
        else if((low_region.curr + PAGE_SIZE < low_region.end))
        {
            pg_start = low_region.curr;
            low_region.curr += PAGE_SIZE;
        }
        else if((high_region.curr < high_region.end))
        {
            //printk("page_start=%p\n",pg_start);
            pg_start = high_region.curr;
            high_region.curr += PAGE_SIZE;
        }
        // no free memory
        else
        {
                printk("Ran out of physical  memory\n");
                bail();
        }
        num_frames_total--;
        return pg_start;
}

// frees an entire physical frame
int pf_free(void *frame_start)
{
        // enqueue the new frame to the free list
        if(free_head == INVALID_START_ADDR)
        {
                if(DBUG) printk("First free!\n");
                free_head = frame_start;
                memset(free_head,0xFF,sizeof(void *));
        }
        else
        {
                memcpy(frame_start,&free_head,sizeof(void *));
                free_head = frame_start;
        }
        if(DBUG) printk("pf_free: old head = %lx,new_head = %p\n",*(uint64_t *)free_head,free_head);
        return SUCCESS;
}
void pf_simple_test()
{
        #define COUNT 10
        void *pages[COUNT];
        printk("pf_simple_test:\n");
        for(int i = 0;i<COUNT;i++)
        {
                pages[i] = pf_alloc();
                printk("Alloc %d: page addr = %p\n",i+1,pages[i]);
                /*
                if(!((i+1)%COUNT00))
                        printk("Allocated 1k, %d in total so far\n",i+1);
                        */
        }
        
        for(int i = 0;i<COUNT;i++)
        {
                pf_free(pages[i]);
        }

        /*
        printk("%p\n",page);
        printk("%p\n",low_region.start+158*PAGE_SIZE);
        printk("%p\n",high_region.start);
        */
        return;
}

void pf_nonseq_test()
{
        #define COUNT 10
        void *pages[COUNT];
        printk("pf_nonseq_test:\n");
        printk("free_head = %p\n",free_head);
        for(int i = 0;i<COUNT;i++)
        {
                pages[i] = pf_alloc();
                printk("Alloc %d: page addr = %p\n",i+1,pages[i]);
                /*
                if(!((i+1)%COUNT00))
                        printk("Allocated 1k, %d in total so far\n",i+1);
                        */
        }
        
        for(int i = 1;i<COUNT;i+=2)
        {
                pf_free(pages[i]);
        }
        for(int i = 0;i<COUNT;i++)
        {
                pages[i] = pf_alloc();
                printk("Alloc %d: page addr = %p\n",i+1,pages[i]);
                /*
                if(!((i+1)%COUNT00))
                        printk("Allocated 1k, %d in total so far\n",i+1);
                        */
        }

        /*
        printk("%p\n",page);
        printk("%p\n",low_region.start+158*PAGE_SIZE);
        printk("%p\n",high_region.start);
        */
        return;
}

int pf_stress_test()
{
        printk("num_frames_low = %d,num_frames_high = %d\n",num_frames_low,num_frames_high);
        printk("num frames total = %d\n",num_frames_total);
        uint8_t bitmap[PAGE_SIZE];
        void *page_start;
        void *region_start = low_region.start;
        int num_frames = num_frames_low; // for validation purposes
        int original_total_frames = num_frames_total;
        for(int i = 0; i<original_total_frames;i++)
        {
                /*
                if(i==160)
                {
                        int loop=0;
                        while(!loop);
                }
                */
                page_start = pf_alloc();
                if(!page_start) 
                {
                        printk("null page start\n");
                        hlt();
                }
                if(DBUG) printk("page_start %d = %p\n",i+1,page_start); 
                for(int j = 0;j<PAGE_SIZE;j+=sizeof(void *))
                {
                        if((err=(uint64_t)memcpy(bitmap+j,&page_start,sizeof(void *))) < 0)
                        {
                                printk("pf_stress_test: memcpy error\n");
                                return err;
                        }
                }
                /* write the bit pattern to the page */
                        if((err=(uint64_t)memcpy(page_start,bitmap,PAGE_SIZE)) < 0)
                        {
                                printk("pf_stress_test: memcpy error\n");
                                return err;
                        }
        }
        printk("num freames = %d\n",num_frames_total);
        if(DBUG) printk("done writing bit patterns\n");
        /* try to allocate more frames than are available in RAM */
        if(pf_alloc() != INVALID_START_ADDR)
        {
                printk("pf_stress_test: Error with overallocation. Num frames remaining = %d\n",num_frames_total);
                return ERR_PF_TEST; 
        }
        for(int j=0;j<2;j++)
        {
        /* validate all frames in low region */
        for(int i=0;i<num_frames;i++)
        {
                page_start = region_start + i*PAGE_SIZE;
                for(int j = 0;j<PAGE_SIZE;j+=sizeof(void *))
                {
                        if((err=(uint64_t)memcpy(bitmap+j,&page_start,sizeof(void *))) < 0)
                        {
                                printk("pf_stress_test: memcpy error\n");
                                return err;
                        }
                }
           if(!are_pages_equal(page_start,bitmap))
           {
                   printk("pf_stress_test: validation error for page %d: %p\n",i+1,page_start);
                   return -1;
           }
           //else printk("pf_stress_test: SUCCESS for page %d, %p\n",i+1,page_start);
        }
        printk("low region validation complete!\n");
        region_start = high_region.start;
        num_frames = num_frames_high;
        }

        printk("high region validation complete!\n");
        /* validate all frames in high region */
        return SUCCESS;
}

int are_pages_equal(const void *ptr1, const void *ptr2) {
    const unsigned char *byte_ptr1 = ptr1;
    const unsigned char *byte_ptr2 = ptr2;

    for (size_t i = 0; i < PAGE_SIZE; ++i) {
        if (byte_ptr1[i] != byte_ptr2[i]) {
            if(DBUG)
            {
                    printk("bytes %ld differ: for ptr1 = %d,ptr2=%d\n",i,byte_ptr1[i],byte_ptr2[i]);
            }
            return 0; // Not equal
        }
    }
    return 1; // Equal
}

void *page_align_up(void *addr)
{
        uint64_t remainder;
        if((remainder = (uint64_t)addr % PAGE_SIZE)) 
        {
                addr += PAGE_SIZE - remainder;
        }
        return addr;
}

/*
 * Initializes fixed size allocator pools 
 * Params:
 * Returns:
 * status code
 */
int init_pools()
{
    const int POOL_SIZES[] = {32, 64, 128, 512, 1024, 2048};
    int curr_size = -1;
    // allocate a frames worth of blocks for each pool
    for(int i=0;i<NUM_POOLS;i++)
    {
        curr_size = POOL_SIZES[i];
        ram_pools[i].max_size = curr_size;
        ram_pools[i].avail = INITIAL_BLOCKS_PER_POOL;
        ram_pools[i].head = alloc_pool_block(curr_size);

    }

    return SUCCESS;
}

/*
 * Dynamically allocates a pool of the requested size 
 * Params:
 * size -- pool size
 * Returns:
 * VA corresponding to start of the block
 */
void *alloc_pool_block(int size)
{
        void *block_start = MMU_alloc_page(); // VA  of first block in pool
        uint64_t *temp = NULL;
        int num_blocks = PAGE_SIZE/size;
        if(!(va_to_pa((void *)block_start,NULL,SET_PA)))
        {
                printk("alloc_pool: va_to_pa() failed for size = %d\n",size);
                bail();
        }
        // partition the frame into multiple blocks
        for(int i=0;i<num_blocks;i++)
        {
            temp = (uint64_t *)((uint64_t)block_start + (size * i));
            if(i != num_blocks-1)
                    *temp = (uint64_t)temp+size;
            else
                    *temp = 0; // zero out next pointer for last block
        }

        return block_start;
}

/*
 * Dynamically allocates virtual memory 
 * Params:
 * size -- number of bytes requested
 * Returns:
 * pointer to start of usable allocated region
 */
void *kmalloc(size_t size)
{
    void *start_addr;
    size += sizeof(KmallocExtra);
    int num_pages = size/PAGE_SIZE;
    // allocate an extra page
    if(size%PAGE_SIZE) num_pages++;

    start_addr = MMU_alloc_pages(num_pages);
    return start_addr;
}

void kfree(void *addr)
{
}
