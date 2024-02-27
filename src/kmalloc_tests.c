/*
 * Nakul Nayak
 * Kernel Project
 * Description: Unit tests for kmalloc and kfree
*/

#include <stdint-gcc.h>
#include "tests.h"
#include "print.h"
#include "constants.h"
#include "utility.h"
#include "error.h"
#include "memutils.h"

#define DBUG 1

extern const int POOL_SIZES[];
extern const size_t KMALLOC_EXTRA_SIZE;

/*
 * Description: Validate the kmalloc header pool and size
 * Params:
 * addr -- VA returned from a kmalloc() call
 * usable_size -- size requested to kmalloc()
 */ 
int validate_header(void *addr,int usable_size)
{
        KmallocExtra *hdr = (KmallocExtra *)((uint64_t)addr - (uint64_t)KMALLOC_EXTRA_SIZE);
        printk("hdr = %p, hdr_addr = %lx\n",hdr,(uint64_t)addr - (uint64_t)KMALLOC_EXTRA_SIZE);
        int status = SUCCESS; 
        for(int i=0;i<NUM_POOLS;i++)
        {
                if(usable_size <= POOL_SIZES[i])
                {
                        status = assert(hdr->pool_index==i && hdr->usable_size == usable_size);
                        return status;
                }
        }
        // raw paging
        status = assert(hdr->pool_index == -1 && hdr->usable_size == usable_size);
        return status;
}

/*
 * Simple test -- alloc a block, free it, alloc again and check that we’re 
 * given the same block
 */
static int simple()
{
        int status = SUCCESS;
        int *first_addr = kmalloc(32);
        int *second_addr = NULL;
        // write a couple bytes
        *first_addr = 2;
        first_addr[31] = 9;
        status = assert(first_addr[0]==2);
        status = assert(first_addr[31]==9);
        kfree(first_addr);
        second_addr = kmalloc(32);
        kfree(second_addr);
        status = assert(first_addr==second_addr);
        return status;
}

/*
 * 
 */
static int alloc_each_pool()
{
        int status = SUCCESS;
        int *first_addr;
        int *second_addr;
        uint8_t bitmap[PAGE_SIZE];
        int size;
        for(int i=0;i<NUM_POOLS;i++)
        {
            printk("alloc_each_pool: mallocing for pool %d\n",i);
            size = POOL_SIZES[i]-KMALLOC_EXTRA_SIZE;
            first_addr = kmalloc(size);
            if(write_bitmap(bitmap,first_addr,POOL_SIZES[i]) < 0)
            {
                printk("alloc_each_pool: write_bitmap() failed\n");
                bail();
            }
                if(!are_buffers_equal(first_addr,bitmap,POOL_SIZES[i]))
                {
                    printk("alloc_each_pool: validation error for pool %d: %p\n",i,first_addr);
                    return -1;
                }
            kfree(first_addr);
            second_addr = kmalloc(size);
            if((status = assert(first_addr==second_addr)) < 0)
            {
                    printk("pool %d error. size = %d, first=%p,second=%p\n",i,size,first_addr,second_addr);
                    return status;
            }
        }
        return status;
}

/*
 * Alloc and free odd numbers of bytes
 */
static int ugly_sizes()
{
        int status = SUCCESS;
        int *addrs[5];
        int sizes[] = {5,35,89,987,5003};
        int num_sizes = sizeof(sizes)/sizeof(int);
        for(int i=0;i<num_sizes;i++)
        {
                addrs[i] = kmalloc(sizes[i]);
                // validate the headers
                if((status = validate_header(addrs[i],sizes[i])) < 0)
                    return status;
        }
        return SUCCESS;
}

int kmalloc_tests()
{
    int status = SUCCESS;
    if((status = simple()) < 0) 
    {
            printk("failed simple_test\n");
            return status;
    }
    if((status = alloc_each_pool()) < 0) 
    {
            printk("failed alloc_each_pool\n");
            return status;
    }
    if((status = ugly_sizes()) < 0) 
    {
            printk("failed ugly_sizes\n");
            return status;
    }
    printk("kmalloc_tests: SUCCEESS\n");
    return status;
}
