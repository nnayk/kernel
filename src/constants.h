/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#define NUM_IRQS 256
#define KERNEL_CS 0x8
#define CURR_STACK 0
#define KERNEL_MODE 0
#define USER_MODE 3
#define PIT_IRQ_NO 0
#define PIT_INT_NO 0x20+PIT_IRQ_NO
#define KBD_IRQ_NO 1
#define KBD_INT_NO 0x20+KBD_IRQ_NO
#define PIC_OFF_1 0x20
#define PIC_OFF_2 0x28
