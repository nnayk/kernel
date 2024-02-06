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
   irq_clear_mask(COM1_IRQ_NO);
   //int loop = 0;
   //while(!loop);
   asm volatile("int $0x24");
}

int serial_consume(State *state)
{
        // empty buffer
        if (state->consumer == state->producer)  return ERR_BUFF_EMPTY;
        // this is a wrapper around outb that writes to serial output
        outb(PORT_COM1,*state->consumer++);
        // wrap consumer pointer back to "front" of circular queue
        if (state->consumer >= &state->buff[BUFF_SIZE]) 
                state->consumer = &state->buff[0];
        return SUCCESS; 
}

 int serial_write(char toAdd, State *state)
{
        int int_enabled = 0;
        if((int_enabled = are_interrupts_enabled()))
            cli();
        // full buffer, throw away the input char and return.
        if ((state->producer == (state->consumer - 1)) ||
        (state->consumer == &(state->buff[0]) && state->producer == &(state->buff[BUFF_SIZE-1])))
        {
            if(int_enabled) sti();
            return ERR_BUFF_FULL;
        }
        *state->producer++ = toAdd;
        if (state->producer >= &state->buff[BUFF_SIZE])
        {
            if(int_enabled) sti();
            state->producer = &state->buff[0];
        }
        return SUCCESS; 
}
