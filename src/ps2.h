/*
 * Nakul Nayak
 * Kernel Project
 * Description: PS/2 controller API interface
*/
#include <stdint-gcc.h>
void ps2_init();
void kbd_init();
char ps2_poll_read();
void ps2_poll_write(uint16_t,uint8_t);
void ps2_enable_kbd_int();
void kbd_isr(int,int,void *);
