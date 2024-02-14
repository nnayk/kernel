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

static int num_pt_entries = PAGE_SIZE/sizeof(void *);
static int err;

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

    entry->address = RSHIFT_ADDR(phys_addr);
    printk("phys_addr = %p,entry->addr = %lx\n",
                    phys_addr,(long)entry->address);

    return SUCCESS;
}

// translates a virtual address to physical address
void *va_to_pa(void *va)
{
        void *ptr4_addr; // address of page table level 4 for current thread
        if(!va) 
        {
            if(DBUG) printk("va_to_pa: ERR_NULL_PTR");
                
                return ERR_NULL_PTR;
        }

        if(!(pt4_addr = get_pt4_addr()))
        {
                hlt();
        }
}

void *get_pt4_addr()
{

}
