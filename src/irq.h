/*
 * Nakul Nayak
 * Kernel Project
 * Description: API interface for irq
*/

#include "utility.h"
#include "constants.h"

extern char idt[];
typedef struct
{
        uint16_t limit;
        void *base_addr;
}__attribute__((__packed__)) idtr_t;

typedef struct {
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    cs;    // The GDT segment selector that the CPU will 
                              // load into CS before calling the ISR
	uint8_t	    ist:3;        // The IST in the TSS that the CPU will load 
                              //   into RSP; set to zero for now
	uint8_t	    reserved_1:5;          
	uint8_t     type:4;   // gate type (interrupt or trap)
	uint8_t     zero:1;   // fixed at zero
	uint8_t     dpl:2;   // data protection level (0=kernel,3=user)
    uint8_t     present:1; // 1 = valid table entry
	uint16_t    isr_mid;      // The higher 16 bits of the lower 32 bits of the ISR's address
	uint32_t    isr_high;     // The higher 32 bits of the ISR's address
	uint32_t    reserved_2;     // Set to zero
} __attribute__((packed)) idt_entry_t;

int irq_init(void);
void irq_set_mask(int irq);
void irq_clear_mask(int irq);
uint8_t irq_get_mask(int irqline);
void irq_end_of_interrupt(int irq);
typedef void (*irq_handler_t)(int, int, void*);
void irq_set_handler(int irq, irq_handler_t handler, void *arg);
int are_interrupts_enabled();
void load_idtr(idtr_t *);
int irq_helper_init(void);

typedef void (*irq_handler_t)(int, int, void*); 

typedef struct { 
        void *arg; 
        irq_handler_t handler; 
} irq_helper_t[NUM_IRQS];


#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */
#define PIC_EOI		0x20

