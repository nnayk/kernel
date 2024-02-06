/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include "serial.h"
#include "utility.h"
#include "error.h"
#include "irq.h"

#define PORT_COM1 0x3f8          // COM1

static void consume_byte();
static void consume_byte(State *);
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
   outb(PORT_COM1 + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
   outb(PORT_COM1+1,0x02); // enable TX interrupts only
   //int loop = 0;
   //while(!loop);
   irq_clear_mask(COM1_IRQ_NO);
   //asm volatile("int $0x24");
}

void serial_consume(State *state)
{
        // line status interrupt
        if(is_line_int())
        {
                // read LSR to clear the int.
                read_LSR(); 
        }
        // empty buffer, turn off TX interrupts and set idle bit
        else if (state->consumer == state->producer)  
        {
                irq_set_mask(COM1_IRQ_NO);
                state->idle = 1;
        }
        else
        {
            // this is a wrapper around outb that writes to serial output
            consume_byte(state); 
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
        if(state->idle && get_hw_buff_status())
        {
                consume_byte(state);
        }


        // TODO: fix this check. full buffer, throw away the input char and return.
        if ((state->producer == (state->consumer - 1)) ||
        (state->consumer == &(state->buff[0]) && state->producer == &(state->buff[BUFF_SIZE-1])))
        {
            if(int_enabled) sti();
            return ERR_BUFF_FULL;
        }
        *state->producer++ = toAdd;

        if (state->producer >= &state->buff[BUFF_SIZE])
        {
            state->producer = &state->buff[0];
        }
        if(int_enabled) sti();
        return SUCCESS; 
}

static void consume_byte(State *state)
{
    state->idle = 0;
    irq_clear_mask(COM1_IRQ_NO);
    outb(PORT_COM1,*state->consumer++);
    // wrap consumer pointer back to "front" of circular queue
    if (state->consumer >= &state->buff[BUFF_SIZE]) 
        state->consumer = &state->buff[0];
}

// Returns >1 if HW buffer is empty, 0 if not.
static int get_hw_buff_status()
{
    uint8_t status = read_LSR();;
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
