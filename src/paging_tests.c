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
                printk("pf_stress_test: write_bitmap() error\n");
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
        for(int i = 0;i<COUNT;i++)
        {
                pages[i] = MMU_alloc_page();
                va_to_pa(pages[i],NULL,SET_PA);
                printk("Alloc %d: page addr = %p, frame addr = %p\n",i+1,pages[i],va_to_pa(pages[i],NULL,GET_PA));
        }
        // validate the frames and free the pages

        
        // confirm that the pages map to null again

        return SUCCESS;
}

int paging_stress_test()
{
        printk("num_frames_low = %d,num_frames_high = %d\n",num_frames_low,num_frames_high);
        printk("num frames total = %d\n",num_frames_total);
        uint8_t bitmap[PAGE_SIZE];
        void *page_start;
        void *region_start = ram[REGION0_OFF].start;
        int num_frames = num_frames_low; // for validation purposes
        int original_total_frames = num_frames_total;
        for(int i = 0; i<original_total_frames;i++)
        {
                page_start = pf_alloc();
                if(DBUG) printk("page_start %d = %p\n",i+1,page_start);
                if(write_bitmap(bitmap,page_start,PAGE_SIZE) < 0)
                {
                        printk("pf_stress_test: write_bitmap() error\n");
                        bail();
                }
        }
        printk("num frames left = %d\n",num_frames_total);
        if(DBUG) printk("done writing bit patterns\n");
        for(int j=0;j<2;j++)
        {
        /* validate all frames in low region */
        for(int i=0;i<num_frames;i++)
        {
                page_start = region_start + i*PAGE_SIZE;
                if(write_bitmap(bitmap,page_start,PAGE_SIZE) < 0)
                {
                        printk("pf_stress_test: write_bitmap() error\n");
                        bail();
                }
                if(!are_buffers_equal(page_start,bitmap,PAGE_SIZE))
                {
                    printk("pf_stress_test: validation error for page %d: %p\n",i+1,page_start);
                    return -1;
                }
        }
        printk("low region validation complete!\n");
        region_start = ram[REGION1_OFF].start;
        num_frames = num_frames_high;
        }

        printk("high region validation complete!\n");
        /* try to allocate more frames than are available in RAM */
        if(pf_alloc() != INVALID_START_ADDR)
        {
                printk("pf_stress_test: Error with overallocation. Num frames remaining = %d\n",num_frames_total);
                return ERR_PF_TEST; 
        }
        /* validate all frames in high region */
        return SUCCESS;
}

void paging_tests()
{
    if(paging_simple_test() < 0)
    {
            printk("paging_tests: failed paging_simple_test\n");
            bail();
    }

    printk("passing paging_tests\n");
}
