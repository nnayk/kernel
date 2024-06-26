/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include "serial.h"
#include "utility.h"
#include "error.h"
#include "irq.h"
#include "vga.h"

#define PORT_COM1 0x3f8          // COM1
#define DBUG 0

static void consume_byte(char);
static int get_hw_buff_status();
static int is_line_int();
static uint8_t read_LSR();
extern State serial_buffer;

void serial_init()
{
   outb(PORT_COM1 + 1, 0x00);    // Disable all interrupts
   outb(PORT_COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT_COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(PORT_COM1 + 1, 0x00);    //                  (hi byte)
   outb(PORT_COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT_COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(PORT_COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   //outb(PORT_COM1 + 4, 0x1E);    // Set in loopback mode, test the serial chip
   outb(PORT_COM1+1,0x02);       // enable TX interrupts only
   //int loop = 0;
   //while(!loop);
   //outb(PORT_COM1 + 4, 0x0F);
   irq_clear_mask(COM1_IRQ_NO);
   //asm volatile("int $0x24");
}

void serial_consume(int int_num,int error_code,void *arg)
{
        if(DBUG) VGA_display_str("Inside consumer!\n");
        State *state = (State *)arg;
        // line status interrupt
        if(is_line_int())
        {
                if(DBUG) VGA_display_str("LSR\n");
                // read LSR to clear the int.
                read_LSR(); 
        }
        // empty buffer, turn off TX interrupts and set idle bit
        else if (buff_empty(state))  
        {
                //if(DBUG) VGA_display_str("EMPTY BUFFER!\n");
                irq_set_mask(COM1_IRQ_NO);
                state->idle = 1;
        }else
        {
            if(DBUG) VGA_display_str("CALLING CONSUME FROM CONSUME!\n");
            // this is a wrapper around outb that writes to serial output
            consume_byte(buff_read(state)); 
            state->idle = 1;
        }
        irq_end_of_interrupt(COM1_IRQ_NO); 
        return; 
}

int serial_write(char toAdd, State *state)
{
        int int_enabled = 0;
        if((int_enabled = are_interrupts_enabled()))
            cli();

        // if hw buffer is empty, write the next byte immediately
        if(get_hw_buff_status())
        {
                if(DBUG) VGA_display_str("hw empty, writing!\n");
                irq_clear_mask(COM1_IRQ_NO);
                consume_byte(toAdd);
                state->idle = 0;
                if(int_enabled) sti();
                return SUCCESS; 
        }
        
        if (buff_full(state))
        {
            if(DBUG) VGA_display_str("buffer full!\n");
            if(int_enabled) sti();
            return ERR_BUFF_FULL;
        }else
        {
            if(DBUG) VGA_display_str("writing to buffer\n");
            buff_write(state,toAdd);
        }
        if(int_enabled) 
        {
            if(DBUG) VGA_display_str("enabling ints\n");
            sti();
        }
        return SUCCESS; 
}

static void consume_byte(char c)
{
    if(DBUG) 
    {
            VGA_display_str("consuming char:");
            VGA_display_char(c);
            VGA_display_char('\n');
    }
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
    return (status == 0x06);
}

static uint8_t read_LSR()
{
    return inb(PORT_COM1+5);
}
