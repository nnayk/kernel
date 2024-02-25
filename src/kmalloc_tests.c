/*
 * Nakul Nayak
 * Kernel Project
 * Description: Unit tests for kmalloc and kfree
*/

#include <stdint-gcc.h>
#include "print.h"
#include "constants.h"
#include "utility.h"
#include "error.h"
#include "memutils.h"

/*
#define assert(condition) \
    do { \
        if (!(condition)) { \
             printk("Assertion failed!\n"); \
        } \
    } while (0)
    */

//#define assert(condition) do { if (ASSERT_SUCCESS(condition) == -1) return -1; } while(0)

/* Simple test -- alloc a block, free it, alloc again and check that we’re 
 * given the same block
*/
static int simple_test()
{
        int status = SUCCESS;
        int *x = kmalloc(32);
        printk("x addr = %p\n",x);
        *x = 2;
        x[31] = 9;
        printk("x[0] = %d, x[31] = %d\n",x[0],x[31]);
        status = assert(x[0]==2);
        return status;
}

int kmalloc_tests()
{
    int status = SUCCESS;
    if((status = simple_test()) < 0) return status;
    printk("kmalloc_tests: SUCCEESS\n");
    return status;
}
