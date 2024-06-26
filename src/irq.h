/*
 * Nakul Nayak
 * Kernel Project
 * Description: API interface for irq
*/

#include "utility.h"
#include "constants.h"

extern char idt[];
extern uint8_t gdt64[];
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
void load_idtr(idtr_t);
int irq_helper_init(void);
void pic_remap(int,int);
void c_wrapper(int,int,void *);
void display_idt_entry(idt_entry_t);

typedef void (*irq_handler_t)(int, int, void*); 

typedef struct { 
        void *arg; 
        irq_handler_t handler; 
} irq_helper_t;

typedef struct {
	uint16_t    seg_low;      // lower 2 bytes of the TSS size
	uint16_t    base_low;     // lower 2 bytes of TSS base address   
	uint8_t	    base_mid_1;   // middle byte of TSS base address 
	uint8_t	    type:4;       // fixed to 0x9 to indicate long mode 
	uint8_t	    zero:1;          
	uint8_t     dpl:2;        // data protection level (0=kernel,3=user)
    uint8_t     present:1;    // 1 = valid table entry
	uint8_t     seg_high:4;   // higher 4 bits of TSS size
    uint8_t     avl:3;        // set to zero
    uint8_t     granularity:1;// set to zero
	uint8_t     base_mid_2;   // upper middle byte of TSS base address
    uint32_t    base_high;
	uint32_t    reserved_2;   // Set bits 8-1 to zero
} __attribute__((packed)) tss_desc_t;

typedef struct{
    uint32_t reserved_1;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved_2;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved_3;
    uint16_t reserved_4;
    uint16_t io_map_base;
} __attribute__((packed)) tss_t;

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

