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

#define RSHIFT_ADDR(ptr) ((uintptr_t)(ptr) >> 12)
#define LSHIFT_ADDR(ptr) ((uintptr_t)(ptr) << 12)


int allocate_pte(PTE_t *entry)
{
    printk("entry = %p\n",entry);
    void *phys_addr;
    if(!entry)
    {
            return ERR_NULL_PTR;
    }
    
    if(!entry->present)
    {
            return ERR_ABSENT_PTE;
    }

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
