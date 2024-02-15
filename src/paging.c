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
#define DBUG 1
#define RSHIFT_ADDR(ptr) ((uintptr_t)(ptr) >> 12)
#define LSHIFT_ADDR(ptr) ((uintptr_t)(ptr) << 12)

//static int num_pt_entries = PAGE_SIZE/sizeof(void *);
static int err;

extern region low_region;
extern region high_region;

int valid_phys_addr(void *);

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
    if(!(phys_addr = pf_alloc()))
    {
            printk("uh oh, phys = %p\n",phys_addr);
            return ERR_NO_MEM;
    }

    entry->addr = RSHIFT_ADDR(phys_addr);
    printk("phys_addr = %p,entry->addr = %lx\n",
                    phys_addr,(long)entry->addr);

    return SUCCESS;
}

// translates a virtual address to physical address
void *va_to_pa(void *va)
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
        if((err=valid_phys_addr(p4_addr)) < 0)
        {
                printk("va_to_pa (%d): p4 entry not present!\n",err);
                hlt();
        }
        entry = (PTE_t *)LSHIFT_ADDR(p4_addr+virt_addr->p4_index);

        // get pt3 address and entry (set if not alloced yet)
        if(!entry->present)
        {
                if(DBUG) printk("va_to_pa (%d): p3 entry not present!\n",err);
                alloc_pte(entry,0);
        }
        
        entry = (PTE_t *)LSHIFT_ADDR(entry->addr+virt_addr->p3_index);
        // get pt2 address and entry (set if not alloced yet)
        if(!entry->present)
        {
                if(DBUG) printk("va_to_pa (%d): p2 entry not present!\n",err);
                alloc_pte(entry,0);
        }
        
        entry = (PTE_t *)LSHIFT_ADDR(entry->addr+virt_addr->p2_index);
        
        // get pt1 address and entry (set if not alloced yet)
        if(!entry->present)
        {
                if(DBUG) printk("va_to_pa (%d): p1 entry not present!\n",err);
                alloc_pte(entry,0);
        }
        
        entry = (PTE_t *)LSHIFT_ADDR(entry->addr+virt_addr->p1_index);
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
                }
        }

        return phys_addr;
}

int valid_phys_addr(void *addr)
{
    if (((low_region.start <= addr) && (addr < low_region.end)) && ((high_region.start <= addr) && (addr < high_region.end)))
    {
            if(DBUG) printk("%p is a valid phys addr\n",(void *)addr);
            return 1;
    }

    return ERR_INVALID_ADDR;
}

#if 0
void *MMU_alloc_page()
{
}

void *MMU_alloc_pages(int num)
{
}

void MMU_free_page(void *va)
{
}

void MMU_free_pages(void *va_start, int count)
{
}
#endif
