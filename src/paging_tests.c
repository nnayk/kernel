/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include <stdint-gcc.h>
#include "memutils.h"
#include "constants.h"
#include "error.h"
#include "tests.h"
#include "utility.h"
#include "print.h"
#include "paging.h"

#define DBUG 0

extern region ram[];
extern int num_frames_low;
extern int num_frames_high;
extern int num_frames_total;
extern void *free_head;

int paging_simple_test()
{
        #define COUNT 10
        void *pages[COUNT];
        uint8_t bitmap[PAGE_SIZE];
        printk("pf_simple_test:\n");
        // allocate pages only
        for(int i = 0;i<COUNT;i++)
        {
                pages[i] = MMU_alloc_page();
                va_to_pa(pages[i],NULL,SET_PA);
                printk("Alloc %d: page addr = %p, frame addr = %p\n",i+1,pages[i],va_to_pa(pages[i],NULL,GET_PA));
        }
        for(int i = 0;i<COUNT;i++)
        {
            if(write_bitmap(bitmap,pages[i],PAGE_SIZE) < 0)
            {
                printk("paging_stress_test: write_bitmap() error\n");
                return -1;
            }
            if(!va_to_pa(pages[i],NULL,GET_PA))
            {
                    printk("paging_simple_test: uh oh, no frame for va %p\n",pages[i]);
                    bail();
            }
            if(!are_buffers_equal(bitmap,pages[i],PAGE_SIZE))
            {
                    printk("paging_simple_test: uh oh, page data doesn't match"  
                                    "bitmap for va %p\n",(char *)pages[i]);
                    return -2;
            }
        }
        for(int i=0;i<COUNT;i++) MMU_free_page(pages[i]);
        return SUCCESS;
}

int paging_stress_test()
{
        printk("num frames total = %d\n",num_frames_total);
        uint8_t bitmap[PAGE_SIZE];
        void *page_start;
        int original_total_frames = num_frames_total;
        for(int i = 0; i<original_total_frames;i++)
        {
                page_start = MMU_alloc_page();
                va_to_pa(page_start,NULL,SET_PA);
                //printk("page addr %d = %p, frame addr = %p\n",i+1,page_start,va_to_pa(page_start,NULL,GET_PA));
                if(i%1000==0) 
                    printk("i=%d,num frames left = %d,page addr %d = %p, frame addr = %p,\n",i,num_frames_total,i+1,page_start,va_to_pa(page_start,NULL,GET_PA));
                if(write_bitmap(bitmap,page_start,PAGE_SIZE) < 0)
                {
                        printk("paging_stress_test: write_bitmap() error\n");
                        bail();
                }
                if(!are_buffers_equal(page_start,bitmap,PAGE_SIZE))
                {
                    printk("paging_stress_test: validation error for page %d: %p\n",i+1,page_start);
                    return -1;
                }
        }
        if(DBUG) printk("done writing bit patterns\n");

        return SUCCESS;
}

void paging_tests()
{
    if(paging_simple_test() < 0)
    {
            printk("paging_tests: failed paging_simple_test\n");
            bail();
    }
    paging_stress_test();
    printk("passing paging_tests\n");
}
