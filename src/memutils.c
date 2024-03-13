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
extern region elf_region;
extern region ram[];
extern void *free_head;
extern uint8_t *multiboot_start;
static uint64_t err;
uint32_t num_frames_total; // total number of physical frames
uint32_t num_frames_low; // number of physical frames in low region
uint32_t num_frames_high; // number of physical frames in high region
static KmallocPool ram_pools[NUM_POOLS];
const int POOL_SIZES[] = {32, 64, 128, 512, 1024, 2048};
const size_t KMALLOC_EXTRA_SIZE = sizeof(KmallocExtra);

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
        if(n>LIMIT)
        {
                printk("ooga\n");
                bail();
                return (void *)ERR_INPUT_SIZE;
        }
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
        if(DBUG) printk("multiboot_start = %p\n",multiboot_start);
        // get size of the entire multiboot struct
        uint32_t total_size;
        uint32_t curr_off = 2 * sizeof(uint32_t); // current offset into multiboot struct
        tag_hdr temp;
        uint8_t *addr;

        if((err=(uint64_t)memcpy(&total_size,multiboot_start,sizeof(uint32_t))) <= 0)
                return err;
        if(DBUG) printk("total struct size = %d\n",total_size);
        // loop until tag type = 8 reached
        while(curr_off < total_size)
        {
             addr = multiboot_start+curr_off;
             if(((uint64_t)addr)%8)
             {
                     if(DBUG) printk("not an even multiple!\n");
                     curr_off += (8-(uint64_t)addr%8);
                     addr = multiboot_start+curr_off;
             }
             memcpy(&temp,addr,sizeof(tag_hdr));
             if(DBUG) printk("type = %d\n,size=%hx\n",temp.type,temp.size);
             if(temp.type == MMAP_TAG)
             {
                     if(DBUG) printk("addr arrays = %p\n",addr+16);
                     if((err=(uint64_t)memcpy(memhdr,addr,sizeof(memtag_hdr_t))) < 0)
                             return err;
                     memhdr->entry_start = addr+MEM_INFO_OFF;
                     if(DBUG) printk("addr arrays in struct = %p\n",memhdr->entry_start);
             }
             else if(temp.type == ELF_TAG)
             {
                     if(DBUG) printk("entry 1: type = %d, seg. addr = %p\n",*(uint32_t *)(addr+24),addr+36);
                     if((err=(uint64_t)memcpy(elfhdr,addr,sizeof(elftag_hdr_t))) < 0)
                             return err;
                     elfhdr->entry_start = addr+SEC_INFO_OFF;
             }
             else if(temp.type == 4)
        {
                if(DBUG) printk("low mem size = %d, high mem size = %d\n",(uint32_t)*(addr+8),(uint32_t)*(addr+12));
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
        uint8_t region0_set = 0; // need to use this var. b/c low region starts at 0x0
        memtag_entry_t mementry;
        elf_entry_t elfentry;
        if(DBUG) 
        {
                printk("inside setup_unused\n");
                printk("addr arrays in struct = %p,version=%d\n",mmaphdr.entry_start,mmaphdr.entry_version);
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
                        if(!region0_set)
                        {
                                region0_set = 1;
                                ram[REGION0_OFF].start = mementry.start_addr;
                                if(!ram[REGION0_OFF].start)
                                        ram[REGION0_OFF].start += PAGE_SIZE;
                                ram[REGION0_OFF].curr = ram[REGION0_OFF].start;
                                ram[REGION0_OFF].end = ram[REGION0_OFF].start+mementry.size; //exclusive end address
                                num_frames_low = mementry.size/PAGE_SIZE;
                                num_frames_total = num_frames_low;
                                if(DBUG) printk("Number of frames in low = %d\n",num_frames_total);
                        }
                        else
                        {
                                ram[REGION1_OFF].start = mementry.start_addr;
                                ram[REGION1_OFF].curr = ram[REGION1_OFF].start;
                                ram[REGION1_OFF].end = mementry.start_addr+mementry.size;
                                ram[REGION0_OFF].next = &ram[REGION1_OFF];
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
                    if(DBUG) printk("Found the kernel code!\n");
                    if(DBUG)
                    {
                            // check no high region overlap
                            if(ram[REGION1_OFF].start < elfentry.seg_addr && ram[REGION1_OFF].end < elfentry.seg_addr)
                                    printk("No overlap for high region\n");
                    }
                }
                elf_addr += elfhdr.entry_size;
        }
        
        elf_region.end = elfentry.seg_addr+elfentry.seg_size;
#endif
        // handle the memory and elf overlap
        // check no low region overlap
        if(ram[REGION0_OFF].start < elf_region.start && ram[REGION0_OFF].start < elf_region.end)
        {
                if(DBUG) printk("No low region overlap!\n");
        }
        else
        {
                if(DBUG) printk("There IS low region overlap!\n");
        }
        // check no high region overlap
        if(ram[REGION1_OFF].start < elf_region.start && ram[REGION1_OFF].end < elf_region.end)
        {
                if(DBUG) printk("No high region overlap!\n");
        }
        else
        {
                if(DBUG) printk("There IS high region overlap!\n");
                ram[REGION1_OFF].start = page_align_up(elf_region.end);
                ram[REGION1_OFF].curr = ram[REGION1_OFF].start;
                num_frames_total -= num_frames_high;
                num_frames_high = (ram[REGION1_OFF].end-ram[REGION1_OFF].start)/PAGE_SIZE;
                num_frames_total += num_frames_high;
                if(DBUG) printk("new num_frames_high = %d, num_frames_total = %d\n",num_frames_high,num_frames_total);
                
        }
        ram[REGION1_OFF].next = INVALID_START_ADDR;
        if(DBUG)
        {
            region *temp = &ram[REGION0_OFF];
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
        else if((ram[REGION0_OFF].curr + PAGE_SIZE < ram[REGION0_OFF].end))
        {
            pg_start = ram[REGION0_OFF].curr;
            ram[REGION0_OFF].curr += PAGE_SIZE;
        }
        else if((ram[REGION1_OFF].curr < ram[REGION1_OFF].end))
        {
            //printk("page_start=%p\n",pg_start);
            pg_start = ram[REGION1_OFF].curr;
            ram[REGION1_OFF].curr += PAGE_SIZE;
        }
        // no free memory
        else
        {
                printk("Ran out of physical  memory. num frames left = %d\n",num_frames_total);
                bail();
        }
        num_frames_total--;
        /*
        if((uint64_t)pg_start % PAGE_SIZE)
        {
            printk("pf_alloc: bad frame %p\n",pg_start);
            bail();
        }
        */
        return pg_start;
}

// frees an entire physical frame
int pf_free(void *frame_start)
{
        /*
        if((uint64_t)frame_start % PAGE_SIZE)
        {
            printk("pf_free: bad frame %p\n",frame_start);
            bail();
        }
        */
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
        num_frames_total++;
        return SUCCESS;
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
 * None
 * Returns:
 * status code
 */
int init_pools()
{
    int curr_size = -1;
    // allocate a frames worth of blocks for each pool
    for(int i=0;i<NUM_POOLS;i++)
    {
        curr_size = POOL_SIZES[i];
        ram_pools[i].max_size = curr_size;
        ram_pools[i].avail = PAGE_SIZE/curr_size;
        ram_pools[i].head = alloc_pool_blocks(curr_size);

    }

    return SUCCESS;
}

#if 0
/*
 * Dynamically allocates a pool of the requested size 
 * Params:
 * size -- pool size
 * Returns:
 * VA corresponding to start of the block
 */
Block *alloc_pool_blocks(int size)
{
        Block *block_start = (Block *)MMU_alloc_page(); // VA  of first block in pool
        Block *temp = NULL;
        int num_blocks = PAGE_SIZE/size;
        if(!(va_to_pa((void *)block_start,NULL,SET_PA)))
        {
                printk("alloc_pool: va_to_pa() failed for size = %d\n",size);
                bail();
        }
        // partition the frame into multiple blocks
        for(int i=0;i<num_blocks;i++)
        {
            temp = (Block *)((uint64_t)block_start + (size * i));
            temp->next=(Block *)0xABCD;
            printk("%p\n",(Block *)((uint64_t)temp+size));
            if(i != num_blocks-1)
            {
                    temp->next = (Block *)((uint64_t)temp+size);
            }
                    
            else
                    temp->next = 0; // zero out next pointer for last block
        }

        return block_start;
}
#endif

Block *alloc_pool_blocks(int size)
{
        Block *block_start = (Block *)MMU_alloc_page(); // VA  of first block in pool
        uint64_t *temp = NULL;
        int num_blocks = PAGE_SIZE/size;
        if(!(va_to_pa((void *)block_start,NULL,SET_P1)))
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

void display_pools()
{
    KmallocPool pool;
    Block *block = NULL;
    int count;
    for(int i=0;i<NUM_POOLS;i++)
    {
        count = 0;
        pool = ram_pools[i];
        if(DBUG) printk("Pool %d: size = %d, avail = %d\n",i+1,pool.max_size,pool.avail);
        block = pool.head;
        while(block)
        {
                if(DBUG) printk("Block %d addr = %p\n",++count,block);
                block = block->next;
        }
    }
}

/*
 * Returns the next block in the requested pool (or null if pool is empty) 
 * Params:
 * pool_index
 * Returns:
 * pointer to the block to use, NULL if no blocks left
 */
Block *alloc_block(int pool_index)
{
    if(!ram_pools[pool_index].avail) return NULL;
    Block *head = ram_pools[pool_index].head;
    Block *next = head->next;
    ram_pools[pool_index].head = next;
    ram_pools[pool_index].avail--;
    return head;
}

/*
 * Frees the given block in the requested pool (or null if pool is empty) 
 * Params:
 * pointer to block
 * pool_index
 * Returns:
 * None
 */
void free_block(Block *blk,int pool_index)
{
        if(pool_index < 0 || pool_index > NUM_POOLS)
        {
                printk("free_block: invalid pool_index %d\n",pool_index);
                bail();
        }

        Block *old_head = ram_pools[pool_index].head;
        blk->next = old_head;
        ram_pools[pool_index].head = blk;
        ram_pools[pool_index].avail++;
        if(DBUG) printk("old head = %p, new head = %p\n",old_head,ram_pools[pool_index].head);
}

/*
 * Dynamically allocates memory 
 * Params:
 * size -- number of bytes requested
 * Returns:
 * pointer to start of usable allocated region
 */
void *kmalloc(size_t usable_size)
{
    void *start_addr;
    size_t true_size = usable_size + KMALLOC_EXTRA_SIZE;
    KmallocExtra hdr;
    int num_pages = 0;
    int pool_size = 0;
    int done = 0; // indicator if raw paging is needed
    int i = 0; // for loop below

    if(usable_size <= 0)
    {
            printk("kmalloc: cannot allocate %ld bytes\n",usable_size);
            return NULL;
    }

    // find best fit pool and allocate a block
    while(i<NUM_POOLS && !done)
    {
            pool_size = ram_pools[i].max_size; 
            if(true_size <= pool_size)
            {
                hdr.pool_index = i;
                hdr.usable_size = usable_size;
                if(!(start_addr = (void *)alloc_block(hdr.pool_index)))
                {
                    start_addr = alloc_pool_blocks(pool_size);
                }
                done = 1;
            }
            i++;
    }
    /* requested size is larger than biggest pool, allocate frames (MMU_alloc_pages() + va_to_pa()) */
    if(!done)
    {
        num_pages = (true_size + PAGE_SIZE - 1) / PAGE_SIZE;
        start_addr = MMU_alloc_pages(num_pages);
        /*
        // map the new pages
        for(int i=0;i<num_pages;i++)
        {
            va_to_pa((void *)((uint64_t)start_addr+PAGE_SIZE*i),NULL,SET_PA);
        }
        */
        hdr.pool_index = -1;
        hdr.usable_size = usable_size;
    }

    memcpy(start_addr,&hdr,KMALLOC_EXTRA_SIZE);
    return (void *)((uint64_t)start_addr+KMALLOC_EXTRA_SIZE);
}

/*
 * Frees dynamically allocated memory
 * Params:
 * addr -- starting address of data to free
 * Returns:
 * None
 */
void kfree(void *addr)
{
        if(!valid_va(addr))
        {
                printk("kfree: invalid va %p\n",addr);
                bail();
        }

        KmallocExtra *hdr = (void *)((uint64_t)addr-KMALLOC_EXTRA_SIZE);
        int pool_index = hdr->pool_index;
        int count; // number of blocks to free up (or frames if raw paging was used)
        // raw paging, need to free up each frame
        if(pool_index == -1)
        {
            count = (hdr->usable_size + KMALLOC_EXTRA_SIZE + PAGE_SIZE - 1)/PAGE_SIZE;
            MMU_free_pages((void *)hdr,count);
        }
        // return the blocks to the corresponding pool
        else if((pool_index >=0) && (pool_index <= 5))
        {
            if(DBUG) printk("freeing block w/index = %d\n",hdr->pool_index);
            free_block((void *)hdr,hdr->pool_index); 
        }
        else
        {
            printk("kfree: ERROR -- invalid pool_index = %d,addr = %p\n",pool_index,addr);
            bail();
        }
        // TODO: coalesce with next chunk if possible
        //if(addr + 
        
        // reset header to indicate chunk is free
}

void *krealloc(void *ptr,size_t size)
{
    if(!ptr)
    {
        printk("krealloc: null arg\n");
        return NULL;
    }

    KmallocExtra *hdr = (void *)((uint64_t)ptr-KMALLOC_EXTRA_SIZE);
    void *new_ptr = kmalloc(size);
    memcpy(new_ptr,ptr,hdr->usable_size);
    kfree(ptr);
    return new_ptr;
}

/*
 * Allocates a kernel stack
 * Params:
 * None
 * Returns:
 * pointer to start of stack (i.e. highest address)
 */
void *alloc_kstack()
{
        void *stack_start=(void *)kstack;
        for(int i=0;i<STACK_PAGES;i++)
        {
                if((kstack - STACK_SIZE) <= kheap)
                {
                        printk("alloc_kstack: out of memory!\n");
                        bail();
                }

                VA_t virt_addr = *((VA_t *)&kstack);
                if(DBUG) printk("kstack=%lx, p1 ind = %d\n",kstack,virt_addr.p1_index);
                va_to_pa((void *)kstack,NULL,SET_P1);
                kstack -= PAGE_SIZE;
        }
        if(DBUG) printk("final kstack = %lx\n",kstack);
        return stack_start;
}
