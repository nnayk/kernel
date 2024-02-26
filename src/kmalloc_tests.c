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

static int simple_test()
{
        int status = SUCCESS;
        int *first_addr = kmalloc(32);
        int *second_addr = NULL;
        printk("first_Addr = %p\n",first_addr);
        *first_addr = 2;
        first_addr[31] = 9;
        status = assert(first_addr[0]==2);
        status = assert(first_addr[31]==9);
        kfree(first_addr);
        second_addr = kmalloc(32);
        printk("first_addr = %p, second_addr = %p\n",first_addr,second_addr);
        status = assert(first_addr==second_addr);
        return status;
}

int kmalloc_tests()
{
    int status = SUCCESS;
    if((status = simple_test()) < 0) 
    {
            printk("failed simple_test\n");
            return status;
    }
    printk("kmalloc_tests: SUCCEESS\n");
    return status;
}
