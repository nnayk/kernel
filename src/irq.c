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
#include "serial.h"
#include "memutils.h"
#include "paging.h"
#include "process.h"
#include "vga.h"

#define IDT_ENTRY_SIZE 16
#define TRAP_GATE 0xF
#define INTERRUPT_GATE 0xE
#define DBUG 0
#define NUM_ISTS 6

extern void (*asm_wrappers[NUM_INTS])();
extern State serial_buffer;
extern State kbd_buffer;
idt_entry_t idt_arr[NUM_INTS];
irq_helper_t irq_helper[NUM_INTS];
tss_t tss;
static int err = 0; // error code
uint8_t ists[IST_STACK_SIZE*NUM_ISTS];

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
        if(DBUG) printk("INSIDE IRQ INIT!\n");
        idtr_t idtr;
        tss_desc_t tss_desc;
        size_t tss_size = sizeof(tss);
        size_t tss_desc_size = sizeof(tss_desc);
        size_t idt_size = sizeof(idt_arr);
        uint64_t isr_addr;
        uint64_t tss_addr=(uint64_t)&tss;
        cli();
        pic_remap(PIC_OFF_1,PIC_OFF_2);
        // setup the irq helper
        if(!(err=irq_helper_init()))
        {
            sti();    
            return err;
        }
        // init each idt entry
        for(int i=0;i<NUM_INTS;i++)
        {
            idt_arr[i].cs = KERNEL_CS;
            idt_arr[i].ist = KERNEL_IST;
            idt_arr[i].type = INTERRUPT_GATE;
            idt_arr[i].dpl = KERNEL_MODE;
            idt_arr[i].present = 1;
            isr_addr = (uint64_t)asm_wrappers[i];
            idt_arr[i].isr_low = isr_addr & 0xFFFF;   
            idt_arr[i].isr_mid = (isr_addr >> 16) & 0xFFFF;
            idt_arr[i].isr_high = (isr_addr >> 32) & 0xFFFFFFFF;
        }

        // set the ists for the special faults + traps
        idt_arr[DF_INT_NO].ist = DF_IST;
        idt_arr[PF_INT_NO].ist = PF_IST;
        idt_arr[GPF_INT_NO].ist = GPF_IST;
        idt_arr[YIELD_INT_NO].ist = YIELD_IST;
        idt_arr[KEXIT_INT_NO].ist = KEXIT_IST;
        

        if(DBUG) printk("size of idt: %ld,size of tss_desc: %ld,size of tss:%ld\n", idt_size,tss_desc_size,tss_size);
        // write to the idt memory location
        if(!memcpy(idt,idt_arr,idt_size)) 
        {
            printk("irq_init: memcpy error 1\n");
            bail(); 
        }
        
        idtr.limit = idt_size - 1;
        idtr.base_addr = idt;
        load_idtr(idtr); 
        irq_set_mask(COM1_IRQ_NO);
        irq_set_mask(PIT_IRQ_NO);
        irq_clear_mask(KBD_IRQ_NO);
        ps2_enable_kbd_int();

        // tss descriptor setup
        if(DBUG) printk("tss_size = %d\n",(int)tss_size);
        tss_desc.seg_low = tss_size & 0xFFFF;
        tss_desc.base_low = tss_addr & 0xFFFF;
        tss_desc.base_mid_1 = (tss_addr >> 16) & 0xFF;
        tss_desc.type = 0x9;
        tss_desc.zero = 0;
        tss_desc.dpl = 0;
        tss_desc.present = 1;
        tss_desc.seg_high = (tss_size >> 16) & 0xFF;
        tss_desc.avl = 0;
        tss_desc.granularity = 0;
        tss_desc.base_mid_2 = (tss_addr >> 24) & 0xFF;
        tss_desc.base_high = (tss_addr >> 32) & 0xFFFFFFFF;
        tss_desc.reserved_2 = 0; 
        if(!memcpy(gdt64+16,&tss_desc,16)) 
        {
                printk("irq_init: memcpy err\n");
                bail();
        }
        idtr.limit = idt_size - 1;
        // tss setup
        tss.rsp0 = (uint64_t)&ists[IST_STACK_SIZE]; // TODO: update this value
        tss.rsp1 = 0; 
        tss.rsp2 = 0;
        tss.ist1 = (uint64_t)&ists[IST_STACK_SIZE]; // kernel IST (use for kustack?)
        tss.ist2 = (uint64_t)&ists[IST_STACK_SIZE*2]; // double fault
        tss.ist3 = (uint64_t)&ists[IST_STACK_SIZE*3]; // page fault
        tss.ist4 = (uint64_t)&ists[IST_STACK_SIZE*4]; // GPF
        tss.ist5 = (uint64_t)&ists[IST_STACK_SIZE*5]; // yield
        tss.ist6 = (uint64_t)&ists[IST_STACK_SIZE*6]; // kexit
        tss.ist7 = 0;
        // load tss
        ltr(TSS_DESC_SELECTOR);
        sti();
        //asm volatile("int $0x21");
        if(DBUG) printk("interrupt init. complete\n");
        // delete after kbd ints. work
        int mask = irq_get_mask(1);
        int timer_mask = irq_get_mask(0);
        if(DBUG) printk("kbd mask = %d,timer mask = %d\n",mask,timer_mask);
        if(DBUG) printk("are ints enabled? %d\n",are_interrupts_enabled());
        if(DBUG) 
        {
                printk("idt_arr[14]:\n");
                display_idt_entry(idt_arr[14]);
                display_idt_entry(idt_arr[21]);
        }
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


void load_idtr(idtr_t idtr)
{
        asm("lidt %0" : : "m"(idtr));
}

int irq_helper_init()
{
        for(int i=0;i<NUM_INTS;i++)
        {
                irq_helper[i].handler = NULL;
                irq_helper[i].arg = NULL;
        }
        /* init the entries for the interrupts I'll handle */
        irq_helper[KBD_INT_NO].handler = kbd_isr;
        irq_helper[KBD_INT_NO].arg = &kbd_buffer;
        irq_helper[COM1_INT_NO].handler = serial_consume;
        irq_helper[COM1_INT_NO].arg = &serial_buffer;
        irq_helper[PF_INT_NO].handler = pg_fault_isr;
        irq_helper[YIELD_INT_NO].handler = yield_isr;
        irq_helper[KEXIT_INT_NO].handler = kexit_isr;
        return 1;
}

void c_wrapper(int int_num,int err_code,void *buffer)
{
    if(DBUG && int_num != 36) 
    {
            printk("wrapper: int num. = %d, error = %d\n",int_num,err_code);
    }
    /* validate int. num */
    if(!((0<=int_num) && (int_num < NUM_INTS)))
    {
        VGA_display_str("Error: Invalid interrupt number\n");
        hlt();
    }
    /* check that it's a handled interrupt */
    irq_helper_t entry = irq_helper[int_num];
    if(!entry.handler)
    {
        VGA_display_str("Error: Unhandled interrupt\n");
        if(int_num==ATA1_INT_NO) irq_end_of_interrupt(14);
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
