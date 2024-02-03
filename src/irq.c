/*
 * Nakul Nayak
 * Kernel Project
 * Description: IRQ functions
*/

#include "irq.h"
#include "print.h"
#include "constants.h"
#include "error.h"
#include "isr_asm.h"
#include "ps2.h"
#include "c_isr.h"

#define IDT_ENTRY_SIZE 16
#define TRAP_GATE 0xF
#define INTERRUPT_GATE 0xE
#define DBUG 1


extern void (*asm_wrappers[NUM_IRQS])();
idt_entry_t idt_arr[NUM_IRQS];
irq_helper_t irq_helper[NUM_IRQS];
static int err = 0; // error code

void pic_remap(int offset1, int offset2)
{
	uint8_t a1, a2;

	a1 = inb(PIC1_DATA); // save masks
	a2 = inb(PIC2_DATA);
    // starts the initialization sequence (in cascade mode)
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1); // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2); // ICW2: Slave PIC vector offset
	io_wait();
    // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC1_DATA, 4); 
	io_wait();
    // ICW3: tell Slave PIC its cascade identity (0000 0010)
	outb(PIC2_DATA, 2);                       
	io_wait();
    // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	outb(PIC1_DATA, ICW4_8086);               
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

int irq_init()
{
        printk("INSIDE IRQ INIT!\n");
        idtr_t idtr;
        size_t idt_size = sizeof(idt_arr);
        uint64_t isr_addr;
        cli();
        pic_remap(PIC_OFF_1,PIC_OFF_2);
        // setup the irq helper
        if(!(err=irq_helper_init()))
                return err;
        // init each idt entry
        for(int i=0;i<NUM_IRQS;i++)
        {
            idt_arr[i].cs = KERNEL_CS;
            idt_arr[i].ist = CURR_STACK;
            idt_arr[i].type = INTERRUPT_GATE;
            idt_arr[i].dpl = KERNEL_MODE;
            idt_arr[i].present = 1;
            isr_addr = (uint64_t)asm_wrappers[i];
            idt_arr[i].isr_low = isr_addr & 0xFFFF;   
            idt_arr[i].isr_mid = (isr_addr >> 16) & 0xFFFF;
            idt_arr[i].isr_high = (isr_addr >> 32) & 0xFFFFFFFF;
        }
        printk("size of idt: %ld\n",idt_size);
        // write to the idt memory location
        if(!memcpy(idt,idt_arr,idt_size)) return ERR_MEMCPY; 
        
        idtr.limit = idt_size - 1;
        idtr.base_addr = idt;
        load_idtr(idtr); 
        irq_set_mask(PIT_IRQ_NO);
        irq_clear_mask(KBD_IRQ_NO);
        //int loop = 0;
        //while(!loop);
        ps2_enable_kbd_int(); 
        sti();
        if(DBUG) printk("interrupt init. complete\n");
        // delete after kbd ints. work
        int mask = irq_get_mask(1);
        int timer_mask = irq_get_mask(0);
        printk("kbd mask = %d,timer mask = %d\n",mask,timer_mask);
        if(DBUG) printk("are ints enabled? %d\n",are_interrupts_enabled());
        /*asm volatile (
        "int $0x21"  // Use interrupt number 1 (IRQ 1 for keyboard)
    );*/
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

void load_idtr(idtr_t idtr)
{
        asm("lidt %0" : : "m"(idtr));
}

int irq_helper_init()
{
        for(int i=0;i<NUM_IRQS;i++)
        {
                irq_helper[i].handler = NULL;
                irq_helper[i].arg = NULL;
        }
        /* init the entries for the interrupts I'll handle */
        irq_helper[KBD_INT_NO].handler = kbd_isr;
        return 1;
}

void c_wrapper(int int_num,int err_code,void *buffer)
{
    /* validate int. num */
    if(!((0<=int_num) && (int_num < NUM_IRQS)))
    {
        printk("Error: Invalid interrupt number %d",int_num);
        hlt();
    }
    /* check that it's a handled interrupt */
    irq_helper_t entry = irq_helper[int_num];
    if(!entry.handler)
    {
        printk("Error: Unhandled interrupt %d. Error code = %d\n",
                        int_num,err_code);
        hlt();
    }
    /* call the ISR */
    else (*entry.handler)(int_num,err_code,entry.arg);
}

void display_idt_entry(idt_entry_t entry)
{
    printk("cs = %d, ist = %d, type = %x, dpl = %d, present = %d, \
           low addr = %d, mid addr = %d, high addr = %d\n",entry.cs,entry.ist, \
           entry.type,entry.dpl,entry.present,entry.isr_low,entry.isr_mid, \
           entry.isr_high);
}
