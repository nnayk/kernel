/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include "paging.h"
#include "error.h"
#include "memutils.h"
#include "error.h"
#include "print.h"
#include "constants.h"
#include "utility.h"

#define NEXT_PTE_PRESENT 1
#define NEXT_PTE_ABSENT 0
#define ALLOCED 1
#define NOT_ALLOCED 0
#define PTE_WRITABLE 1
#define DBUG 1
#define RSHIFT_ADDR(ptr) ((uintptr_t)(ptr) >> 12)
#define LSHIFT_ADDR(ptr) ((uintptr_t)(ptr) << 12)

static int num_pt_entries = PAGE_SIZE/sizeof(PTE_t *);
static int err;
static void *kheap; //points to top of kernel heap
static void *kstack; //points to topmost of kernel stack (equivalent to rsp for the most recently allocated kernel stack)

extern region low_region;
extern region high_region;


int alloc_pte(PTE_t *entry, int access)
{
    printk("entry = %p\n",entry);
    void *phys_addr;
    if(!entry)
    {
            if(DBUG) printk("alloc_pte: ERR_NULL_PTR");
            return ERR_NULL_PTR;
    }
    
    entry->present = NEXT_PTE_PRESENT;
    entry->user_access = access;
    phys_addr = pf_alloc();

    entry->addr = RSHIFT_ADDR(phys_addr);
    printk("phys_addr = %p,entry->addr = %lx\n",
                    phys_addr,(long)entry->addr);

    return SUCCESS;
}

// translates a virtual address to physical address
void *va_to_pa(void *va,PT_op op)
{
        void *p4_addr; // address of page table level 4 for current thread
        void *phys_addr;
        VA_t *virt_addr = (VA_t *)va;
        PTE_t *entry;
        if(!va) 
        {
            if(DBUG) printk("va_to_pa: ERR_NULL_PTR");
            return (void *)ERR_NULL_PTR;
        }

        p4_addr = get_p4_addr();
        // p4 table frame must be setup already,else terminate
        if((err=valid_pa(p4_addr)) < 0)
        {
                if(DBUG)
                {
                        printk("va_to_pa (%d): p4 entry not present!\n",err);
                        hlt();
                }
                return NULL;
        }
        // get pt4 entry
        entry = (PTE_t *)get_full_addr(p4_addr,virt_addr->p4_index);

        // get pt3 address and entry (set if not alloced yet)
        if(!entry->present)
        {
                if((op == GET_PA) || (op == GET_P1))
                {
                    if(DBUG)
                    {
                            printk("va_to_pa: P3 entry not set for type %d operation\n",op);
                            hlt();
                    }
                    return NULL;
                }
                else alloc_pte(entry,0);
        }
        
        entry = (PTE_t *)get_full_addr(entry,virt_addr->p3_index);
        // get pt2 address and entry (set if not alloced yet)
        if(!entry->present)
        {
                if((op == GET_PA) || (op == GET_P1))
                {
                    if(DBUG)
                    {
                        printk("va_to_pa: P2 entry not set for type %d operation\n",op);
                    }

                    return NULL;
                }
                else alloc_pte(entry,0);
        }
        entry = (PTE_t *)get_full_addr(entry,virt_addr->p2_index);
        
        // get pt1 address and entry (set if not alloced yet)
        if(!entry->present)
        {
                if((op == GET_PA) || (op == GET_P1))
                {
                    if(DBUG)
                    {
                        printk("va_to_pa: P1 entry not set for type %d operation\n",op);
                        hlt();
                    }
                    return NULL; 
                }
                else if(op == SET_P1) 
                {
                        alloc_pte(entry,0);                
                        entry->alloced = ALLOCED;
                }
        }
        else if(op == GET_P1) return get_full_addr(entry,virt_addr->p1_index);
        
        entry = (PTE_t *)get_full_addr(entry,virt_addr->p1_index);
        // get physical frame (set if not alloced yet)
        if(!entry->present)
        {
                if(DBUG) printk("va_to_pa (%d): frame not present!\n",err);
        //TODO: fix this so that the entry is set to the page starting address (not the full address)
        // identity map if VA corresponds to p4 entry 1
                if(va < (void *)0x10000000000)
                {
                    phys_addr = va;
                    entry->addr = RSHIFT_ADDR(phys_addr);
                }
        // otherwise assign to an arbitrary free frame
                else 
                {
                        alloc_pte(entry,0);
                        phys_addr = get_full_addr(entry,virt_addr->frame_off);
                }
        }

        return phys_addr;
}

// Allocates a frame for level 4 of PT, initializes it, and writes the addr to cr3
void *setup_pt4()
{
    uint64_t *table_start = pf_alloc();
    PTE_t entry;
    entry.present = NEXT_PTE_ABSENT;
    entry.writable = PTE_WRITABLE;
    // disable TLB caching for now
    entry.write_through = 1;
    entry.cache_disabled = 1;
    entry.nx = 1;
    // set each entry in the pt4 frame
    for(int i = 0; i < num_pt_entries; i++)
    {
        if(!memcpy(table_start + i,&entry,sizeof(PTE_t)))
        {
                printk("setup_pt4: memcpy error");
                dbug_hlt(DBUG);
                return NULL;
        }
    }
    set_cr3((uint64_t)table_start);
    return table_start;
}

void *get_full_addr(PTE_t *entry,uint16_t offset)
{
        if(!entry)
        {
                printk("get_full_addr: %d\n",ERR_NULL_PTR);
                dbug_hlt(DBUG);
                return NULL;
        }

        return (void *)(LSHIFT_ADDR(entry->addr)+offset);
}

int valid_pa(void *addr)
{
    if (((low_region.start <= addr) && (addr < low_region.end)) && ((high_region.start <= addr) && (addr < high_region.end)))
    {
            if(DBUG) printk("%p is a valid phys addr\n",(void *)addr);
            return 1;
    }

    return ERR_INVALID_ADDR;
}

int valid_va(void *addr)
{
    if(addr && (addr < VA_MAX)) return SUCCESS;
    return ERR_INVALID_ADDR;
}


void *MMU_alloc_page()
{
    void *addr; 
    if(kheap > kstack)
    {
            if(DBUG) printk("MMU_alloc_page: out of kernel memory\n");
            return NULL;
    }
    addr = kheap;
    va_to_pa(kheap,SET_P1);
    kheap += PAGE_SIZE;
    return addr;
}

// Allocates consecutive pages
void *MMU_alloc_pages(int num)
{
    void *first_page_addr;
    if(!(first_page_addr = MMU_alloc_page()))
    {
            if(DBUG) printk("MMU_alloc_pages: MMU_alloc_page failed");
            return NULL;
    }

    for(int i = 0; i < num - 1; i++)
    {
            if(!(MMU_alloc_page()))
            {
                    if(DBUG) printk("MMU_alloc_pages: MMU_alloc_page failed");
                    return NULL;
            }
    }

    return first_page_addr;
}

void MMU_free_page(void *va)
{
    PTE_t *p1_entry;
    void *frame_start;
    VA_t *virt_addr = (VA_t *)va;
    if(!valid_va(va))
    {
            printk("MMU_free_page: Invalid VA %p\n",va);
            return;
    }

    // mark the p1 entry as not present 
    p1_entry = va_to_pa(va,GET_P1);
    p1_entry->present = NEXT_PTE_ABSENT;
    // free the underlying frame associated with the page
    frame_start = get_full_addr(p1_entry,virt_addr->frame_off);
    if((err = pf_free(frame_start) < 0))
    {
            printk("MMU_free_page (%d): pf_free() failed\n",err);
            dbug_hlt(DBUG);
    }
}

#if 0
void MMU_free_pages(void *va_start, int count)
{
}
#endif
