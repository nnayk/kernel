/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/
#include "process.h"
#include "print.h"
#include "fat.h"
#include "utility.h"

static void test_illegal_seek(void *arg)
{
    char *x = "/parent1/child1";
    uint16_t *y = char_arr_to_uint16_arr(x,strlen(x));
    File *f = open(y);
    if(lseek(f,2) > 0) 
    {
        printk("test_illegal_seek: failed on %s\n",x);
        bail();
    }
    x = "/boot/grub/i386-pc/adler32.mod";
    y=char_arr_to_uint16_arr(x,strlen(x));
    f = open(y);
    if(lseek(f,1397) > 0)
    {
        printk("test_illegal_seek: failed on %s\n",x);
        bail();
    }
    printk("passed test_illegal_seek\n");
}

void vfs_tests()
{
    test_illegal_seek(0);
}
