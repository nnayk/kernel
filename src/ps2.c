#include "stdint-gcc.h"
#include "ps2.h"
#include "print.h"

#define PS2_DATA 0x60
#define PS2_CMD 0x64
#define PS2_STATUS PS2_CMD
#define PS2_STATUS_OUTPUT 1
#define PS2_STATUS_INPUT (1 << 1)
#define PS2_WRITE_CONFIG PS2_DATA
#define PS2_ENABLE_PORT1 0xAE
#define PS2_DISABLE_PORT1 0xAD
#define PS2_DISABLE_PORT2 0xA7
#define PS2_ENABLE_CLOCK1 ~(1<<4)
#define PS2_CONFIG 0x20

#define DBUG 1

//void outb(uint16_t, uint8_t);
//uint8_t inb(uint16_t );

static inline void outb(uint16_t port, uint8_t val)
{
        asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port)
{
        uint8_t ret;
        asm volatile ( "inb %1, %0"
        : "=a"(ret)
        : "Nd"(port) );
        return ret;
}

static char ps2_poll_read(void)
{
        char status = inb(PS2_STATUS);
        while (!(status & PS2_STATUS_OUTPUT))
                status = inb(PS2_STATUS);
        return inb(PS2_DATA);
}

static void ps2_poll_write(uint16_t port,uint8_t val)
{
        char status = inb(PS2_STATUS);
        while (!(status & PS2_STATUS_INPUT))
                status = inb(PS2_STATUS);
        outb(port,val);
}

void ps2_init()
{
        uint8_t ps2_byte; // general command byte
        /* disable 1st port */
        outb(PS2_CMD,PS2_DISABLE_PORT1);
        /* disable 2nd port */
        outb(PS2_CMD,PS2_DISABLE_PORT2);
        /* read controller config. byte */
        outb(PS2_CMD,PS2_CONFIG);
        ps2_byte = ps2_poll_read();
        if(DBUG) printk("ps2 byte = %d\n",ps2_byte);
        if(DBUG) printk("ps2 bit 4 = %d\n",ps2_byte & (1 << 4));
        /* enable the clock on channel 1 */
        ps2_byte &= PS2_ENABLE_CLOCK1;
        // write the configuration byte back out to the PS/2 controller.
        ps2_poll_write(PS2_CMD,PS2_WRITE_CONFIG);
        // enable port 1
        outb(PS2_CMD,PS2_ENABLE_PORT1);
}

