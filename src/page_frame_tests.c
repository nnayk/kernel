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

#define DBUG 0

extern region low_region;
extern region high_region;
extern int num_frames_low;
extern int num_frames_high;
extern int num_frames_total;
extern void *free_head;

void pf_simple_test()
{
        #define COUNT 10
        void *pages[COUNT];
        printk("pf_simple_test:\n");
        for(int i = 0;i<COUNT;i++)
        {
                pages[i] = pf_alloc();
                printk("Alloc %d: page addr = %p\n",i+1,pages[i]);
        }
        
        for(int i = 0;i<COUNT;i++)
        {
                pf_free(pages[i]);
        }

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
        }
        
        for(int i = 1;i<COUNT;i+=2)
        {
                pf_free(pages[i]);
        }
        for(int i = 0;i<COUNT;i++)
        {
                pages[i] = pf_alloc();
                printk("Alloc %d: page addr = %p\n",i+1,pages[i]);
        }

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
                page_start = pf_alloc();
                if(!page_start) 
                {
                        printk("null page start\n");
                        hlt();
                }
                if(DBUG) printk("page_start %d = %p\n",i+1,page_start);
                if(write_bitmap(bitmap,page_start,PAGE_SIZE) < 0)
                {
                        printk("pf_stress_test: write_bitmap() error\n");
                        bail();
                }
        }
        printk("num freames = %d\n",num_frames_total);
        if(DBUG) printk("done writing bit patterns\n");
        printk("low_region.start = %p\n",low_region.start);
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
        region_start = high_region.start;
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


