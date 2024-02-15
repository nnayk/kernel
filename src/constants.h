/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#define NUM_IRQS 256
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
#define PIC_OFF_1 0x20
#define PIC_OFF_2 0x28
#define VA_MAX (void *)0x110000000000 // max virtual address for any VA space

#define PG_SIZE 4096
#define STACK_SIZE 4096
#define INVALID_START_ADDR (void *)(0xFFFFFFFFFFFFFFFF)
