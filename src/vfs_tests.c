/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/
#include "process.h"
#include "print.h"
#include "fat.h"
#include "utility.h"
#include "memutils.h"
#include "md5.h"

static void test_illegal_seek()
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

static void test_read_entire_file(char *name)
{
    uint16_t *real_name = char_arr_to_uint16_arr(name,strlen(name));
    File *f = open(real_name);
    char *dst = kmalloc(f->inode->size);
    read(f,dst,f->inode->size);
    for(int i=0;i<f->inode->size;i++)
    {
        printk("%c",dst[i]);
    }
    lseek(f,2);
    read(f,dst,f->inode->size-2);
    for(int i=0;i<f->inode->size-2;i++)
    {
        printk("%c",dst[i]);
    }
    lseek(f,0);
    read(f,dst,f->inode->size);
    for(int i=0;i<f->inode->size;i++)
    {
        printk("%c",dst[i]);
    }
    uint8_t hash[16];
    md5String(dst,f->inode->size,hash);
    for(int i=0;i<16;i++)
    {
        printk("%hx",hash[i]);
    }
}

void util_vfs_hash_file(char *name)
{
    uint16_t *real_name = char_arr_to_uint16_arr(name,strlen(name));
    File *f = open(real_name);
    char *dst = kmalloc(f->inode->size);
    read(f,dst,f->inode->size);
    uint8_t hash[16];
    printk("hashing %ld byte string\n",f->inode->size);
    md5String(dst,f->inode->size,hash);
    for(int i=0;i<16;i++)
    {
        printk("%hx",hash[i]);
    }
}

void test_hash_several_files()
{
    char *filenames[4] = {"/boot/kernel.bin","/boot/grub/fonts/unicode.pf2",
                          "/boot/grub/i386-pc/adler32.mod","/boot/grub/i386-pc/gcry_blowfish.mod"};
    for(int i=0;i<1;i++)
    {
        util_vfs_hash_file(filenames[i]);    
    }
}

void vfs_tests()
{
    test_illegal_seek();
    test_read_entire_file("nakul.txt");
    test_hash_several_files();
}
