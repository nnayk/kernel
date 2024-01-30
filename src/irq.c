/*
 * Nakul Nayak
 * Kernel Project
 * Description: IRQ functions
*/

#include "irq.h"
#include "print.h"
#include "error.h"
#include "isr_asm.h"

#define IDT_ENTRIES 256
#define IDT_ENTRY_SIZE 16
#define TRAP_GATE 0xF
#define INTERRUPT_GATE 0xE

void pic_remap(int,int);
int set_default_idt_entry(idt_entry_t *);
extern void (*asm_wrappers[NUM_IRQS])(); 

idt_entry_t idt[IDT_ENTRIES];
irq_helper_t irq_helper[IDT_ENTRIES];
static int err = 0; // error code

void pic_remap(int offset1, int offset2)
{
	uint8_t a1, a2;

	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);

	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();

	outb(PIC1_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

int irq_init()
{
        idtr_t idtr;
        idt_entry_t default_entry = {0};
        uint64_t isr_addr;
        cli();
        pic_remap(0x20,0x28);
        load_idtr(&idtr);
        if(!(err=set_default_idt_entry(&default_entry)))
                return err;
        // setup the irq helper
       if(!(err=irq_helper_init()))
                return err;
       for(int i=0;i<NUM_IRQS;i++)
       {
            idt[i] = default_entry;
            // set correct isr address and ist
            isr_addr = (uint64_t)asm_wrappers[i];
            idt[i].isr_low = isr_addr & 0xFFFF;   
            idt[i].isr_mid = (isr_addr >> 16) & 0xFFFF;
            idt[i].isr_mid = (isr_addr >> 32) & 0xFFFFFFFF;
       }
       
       printk("IDT Base Address: %p\n", idtr.base_addr);
       sti();
       return 1;
}

void irq_set_mask(int irq)
{
    uint16_t port;
    uint8_t value;

    if(irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) | (1 << irq);
    outb(port, value);
}
void irq_clear_mask(int irq)
{
    uint16_t port;
    uint8_t value;
 
    if(irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) & ~(1 << irq);
    outb(port, value);   
}

uint8_t irq_get_mask(int irq)
{
    uint16_t port;
    uint8_t value;
 
    if(irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) & (1 << irq);
    return value;
}

void irq_end_of_interrupt(int irq)
{
    if(irq >= 8)
        outb(PIC2_COMMAND,PIC_EOI);
    outb(PIC1_COMMAND,PIC_EOI);
}

int are_interrupts_enabled()
{
    unsigned long flags;
    asm volatile ( "pushf\n\t"
                   "pop %0"
                   : "=g"(flags) );
    return flags & (1 << 9);
}

void load_idtr(idtr_t *idtr)
{
    asm volatile("sidt %0" : "=m" (*idtr));
}

int set_default_idt_entry(idt_entry_t *entry)
{
    if(!entry) return ERR_NULL_PTR;
    entry->cs = KERNEL_CS;
    entry->ist = KERNEL_STACK;
    entry->type = INTERRUPT_GATE;
    entry->dpl = KERNEL_MODE;
    entry->present = 1;
    return SUCCESS;
}

int irq_helper_init()
{
        return 1;
}
