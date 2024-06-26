#include "ps2.h"
#include "print.h"
#include "utility.h"
#include "irq.h"
#include "process.h"
#include "shared_buff.h"

#define PS2_DATA 0x60
#define PS2_CMD 0x64
#define PS2_STATUS PS2_CMD
#define PS2_STATUS_OUTPUT 1
#define PS2_STATUS_INPUT (1 << 1)
#define PS2_WRITE_CONFIG PS2_DATA
#define PS2_ENABLE_PORT1 0xAE
#define PS2_DISABLE_PORT1 0xAD
#define PS2_ENABLE_INT_PORT1 1
#define PS2_DISABLE_PORT2 0xA7
#define PS2_ENABLE_CLOCK1 ~(1<<4)
#define PS2_ENABLE_CLOCK2 ~(1<<5)
#define PS1_ENABLE_TR_PORT1 (1<<6)
#define PS2_CONFIG 0x20

#define KBD_RESET 0xFF
#define KBD_SCAN_CODE 0xF0
#define RELEASE_KEY_START 0x81

#define DBUG 0

//void outb(uint16_t, uint8_t);
//uint8_t inb(uint16_t );
extern State kbd_buffer;
extern ProcQueue *kbd_blocked;

char ps2_poll_read(void)
{
        char status = inb(PS2_STATUS);
        while (!(status & PS2_STATUS_OUTPUT))
                status = inb(PS2_STATUS);
        //if(DBUG) printk("READ READY!\n");
        return inb(PS2_DATA);
}

void ps2_poll_write(uint16_t port,uint8_t val)
{
        char status = inb(PS2_STATUS);
        while(status & PS2_STATUS_INPUT)
                status = inb(PS2_STATUS);
        //if(DBUG) printk("WRITE READY!\n");
        outb(port,val);
}

void ps2_init()
{
        uint8_t ps2_byte; // general command byte
        /* disable 1st port */
        ps2_poll_write(PS2_CMD,PS2_DISABLE_PORT1);
        /* disable 2nd port */
        ps2_poll_write(PS2_CMD,PS2_DISABLE_PORT2);
        /* read controller config. byte */
        ps2_poll_write(PS2_CMD,PS2_CONFIG);
        ps2_byte = ps2_poll_read();
        if(DBUG) printk("ps2 byte = %d\n",ps2_byte);
        if(DBUG) printk("ps2 bit 4 = %d\n",ps2_byte & (1 << 4));
        /* enable the clock on channel 1 */
        ps2_byte &= PS2_ENABLE_CLOCK1;
        ps2_byte &= ~PS2_ENABLE_INT_PORT1; // disable port 1 interrupts
        ps2_byte |= ~PS2_ENABLE_CLOCK2;
        ps2_byte &= ~(PS2_ENABLE_CLOCK2);
        if(DBUG) printk("writing ps2 config byte = %d\n",ps2_byte);
        // write the configuration byte back out to the PS/2 controller.
        ps2_poll_write(PS2_CMD,PS2_WRITE_CONFIG);
        ps2_poll_write(PS2_DATA,ps2_byte);
        // enable port 1
        ps2_poll_write(PS2_CMD,PS2_ENABLE_PORT1);
        ps2_poll_write(PS2_CMD,PS2_CONFIG);
        ps2_byte = ps2_poll_read();
        if(DBUG) printk("ps2 config byte = %d\n",ps2_byte);

}

void ps2_enable_kbd_int()
{
        uint8_t ps2_byte; // general command byte
        /* read controller config. byte */
        ps2_poll_write(PS2_CMD,PS2_CONFIG);
        ps2_byte = ps2_poll_read();
        if(DBUG) printk("read ps2 config byte = %d\n",ps2_byte);
        ps2_byte |= PS2_ENABLE_INT_PORT1; // disable port 1 interrupts
        if(DBUG) printk("writing ps2 config byte = %d\n",ps2_byte);
        // write the configuration byte back out to the PS/2 controller.
        ps2_poll_write(PS2_CMD,PS2_WRITE_CONFIG);
        ps2_poll_write(PS2_DATA,ps2_byte);
}

void kbd_init()
{
        uint8_t ps2_byte; // general command byte
        // reset the keyboard
        ps2_poll_write(PS2_DATA,KBD_RESET);
        if(DBUG) printk("sent reset kbd command\n");
        ps2_byte = ps2_poll_read();
        if(DBUG) printk("kbd command ack: %x\n",ps2_byte);
        ps2_byte = ps2_poll_read();
        if(DBUG) printk("ps2 keyboard reset status: %x\n",ps2_byte);
        ps2_poll_write(PS2_CMD,PS2_CONFIG);
        ps2_byte = ps2_poll_read();
        if(DBUG) printk("kbd reset ack = %d\n",ps2_byte);
        // set the keyboard to scan code set 1.
        ps2_poll_write(PS2_DATA,KBD_SCAN_CODE);
        ps2_byte = ps2_poll_read();
        if(DBUG) printk("set scan code cmd ack = %x\n",ps2_byte);
        ps2_poll_write(PS2_DATA,0x1);
        ps2_byte = ps2_poll_read();
        if(DBUG) printk("set scan code data ack = %x\n",ps2_byte);
        // check that kbd scan code set = 1
        ps2_poll_write(PS2_DATA,KBD_SCAN_CODE);
        ps2_byte = ps2_poll_read();
        if(DBUG) printk("get kbd scan cmd ack = %x\n",ps2_byte);
        ps2_poll_write(PS2_DATA,0x0);
        ps2_byte = ps2_poll_read();
        if(DBUG) printk("get kbd scan data ack = %x\n",ps2_byte);
        ps2_byte = ps2_poll_read();
        if(DBUG) printk("current kbd scan code = %x\n",ps2_byte);
        // enable keyboard scanning.
        ps2_poll_write(PS2_DATA,0xF4);
        ps2_byte = ps2_poll_read();
        if(DBUG) printk("enable kbd scanning ack = %x\n",ps2_byte);
}

void kbd_isr(int int_num,int err_code,void *buffer)
{
        //printk("inside kbd isr\n");
        unsigned char data = ps2_poll_read();
        // TODO: write the char to kbd buffer instead of this
        irq_end_of_interrupt(KBD_IRQ_NO);
        if(data < RELEASE_KEY_START)
        {
            buff_write(&kbd_buffer,data);
            PROC_unblock_head(kbd_blocked);
        }
}

unsigned char KBD_read(void)
{
        cli();
        unsigned char data;
        while (buff_empty(&kbd_buffer)) 
        {
                PROC_block_on(kbd_blocked, 1);
                cli();
        }
        // read the key code from the buffer
        // process the key code
        // case 1: the key code corresponds to a single byte ascii char
        data = buff_read(&kbd_buffer);
        if(data < RELEASE_KEY_START)
        {
            data = mapScanCodeToAscii(data);
        }
        // TODO case 2: the key code corresponds to a multi byte command. Currently I plan to check if the key code corresponds to end of command. If so, I'll walk through the entire buffer from consumer to producer to form the single char. Note that I'll probably need to use some buff_peek() call instead of buff_read() above to avoid moving the consumer.
        sti();
        return data;
}
