/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include "serial.h"
#include "utility.h"
#include "error.h"
#include "irq.h"
#include "print.h" // DELETE

#define PORT_COM1 0x3f8          // COM1
#define DBUG 0

static void consume_byte(char);
static int get_hw_buff_status();
static int is_line_int();
static uint8_t read_LSR();

void serial_init()
{
   outb(PORT_COM1 + 1, 0x00);    // Disable all interrupts
   outb(PORT_COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT_COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(PORT_COM1 + 1, 0x00);    //                  (hi byte)
   outb(PORT_COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT_COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(PORT_COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outb(PORT_COM1 + 4, 0x1E);    // Set in loopback mode, test the serial chip
   outb(PORT_COM1+1,0x02);       // enable TX interrupts only
   //int loop = 0;
   //while(!loop);
   outb(PORT_COM1 + 4, 0x0F);
   irq_clear_mask(COM1_IRQ_NO);
   //asm volatile("int $0x24");
}

void serial_consume(int int_num,int error_code,void *arg)
{
        if(DBUG) printk("Inside consumer!\n");
        State *state = (State *)arg;
        // line status interrupt
        if(is_line_int())
        {
                if(DBUG) printk("LSR\n");
                // read LSR to clear the int.
                read_LSR(); 
        }
        // empty buffer, turn off TX interrupts and set idle bit
        else if (state->consumer == state->producer)  
        {
                if(DBUG) printk("EMPTY BUFFER!\n");
                irq_set_mask(COM1_IRQ_NO);
                state->idle = 1;
        }else
        {
             if(DBUG) printk("CALLING CONSUME FROM CONSUME!\n");
            // this is a wrapper around outb that writes to serial output
            consume_byte(*state->consumer++); 
            state->idle = 0;
        }
        // wrap consumer pointer back to "front" of circular queue
        if (state->consumer >= &state->buff[BUFF_SIZE])
                state->consumer = &state->buff[0];
        irq_end_of_interrupt(COM1_IRQ_NO); 
        return; 
}

int serial_write(char toAdd, State *state)
{
        int int_enabled = 0;
        int temp = 0; // DELETE 
        if((int_enabled = are_interrupts_enabled()))
            cli();

        // if hw buffer is empty, write the next byte immediately
        if(state->idle && (temp = get_hw_buff_status()))
        {
                irq_clear_mask(COM1_IRQ_NO);
                consume_byte(toAdd);
                state->idle = 0;
                if(int_enabled) sti();
                temp = are_interrupts_enabled();
                return SUCCESS; 
        }
        
        // TODO: fix this check. full buffer, throw away the input char and return.
        if ((state->producer == (state->consumer - 1)) ||
        (state->consumer == &(state->buff[0]) && state->producer == &(state->buff[BUFF_SIZE-1])))
        {
            if(int_enabled) sti();
            return ERR_BUFF_FULL;
        }else
        {
            *state->producer++ = toAdd;
        }

        if (state->producer >= &state->buff[BUFF_SIZE])
        {
            state->producer = &state->buff[0];
        }
        if(int_enabled) sti();
        return SUCCESS; 
}

static void consume_byte(char c)
{
    outb(PORT_COM1,c);
}

// Returns >1 if HW buffer is empty, 0 if not.
static int get_hw_buff_status()
{
    uint8_t status = read_LSR();
    return (status & (1<<5));
}

static int is_line_int()
{
    uint8_t status = inb(PORT_COM1+2);
    return (status & 0x06);
}

static uint8_t read_LSR()
{
    return inb(PORT_COM1+5);
}
