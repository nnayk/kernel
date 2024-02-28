/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#define PAGE_SIZE 4096
#define NUM_INTS 256
#define KERNEL_CS 0x08
#define TSS_DESC_SELECTOR 0x10
#define CURR_STACK 0
#define KERNEL_IST 1
#define DF_IST 2
#define PF_IST 3
#define GPF_IST 4
#define KERNEL_MODE 0
#define USER_MODE 3
#define PIT_IRQ_NO 0
#define PIT_INT_NO 0x20+PIT_IRQ_NO
#define KBD_IRQ_NO 1
#define KBD_INT_NO 0x20+KBD_IRQ_NO
#define COM1_IRQ_NO 4
#define COM1_INT_NO 0x20+COM1_IRQ_NO
#define DF_INT_NO 0x08
#define PF_INT_NO 0x0E
#define GPF_INT_NO 0x0D
#define YIELD_INT_NO 0x48
#define EXIT_INT_NO 0x49
#define PIC_OFF_1 0x20
#define PIC_OFF_2 0x28
#define VA_IDENTITY_MAP_MAX (void *)0x10000000000 // max virtual address for identity map
#define VA_MAX (void *)0x110000000000 // max virtual address for any VA space

#define IST_STACK_SIZE 4096
#define INVALID_START_ADDR (void *)(0xFFFFFFFFFFFFFFFF)

#define KHEAP_START 0x10000000000 // bottom of heap (inclusive)
#define KHEAP_LIMIT 0xF0000000000 // top of heap (exclusive)
#define KSTACK_START 0xFFFFFFFFFFF // bottom of first stack (inclusive)
#define KSTACK_LIMIT 0x100000000000
#define STACK_SIZE 2000000
#define STACK_PAGES STACK_SIZE/PAGE_SIZE
#define MAX_FRAME_ADDR (void *)0xC0000000 // first 3 GB
#define INITIAL_BLOCKS_PER_POOL 1

#define REGION0_OFF 0
#define REGION1_OFF 1
