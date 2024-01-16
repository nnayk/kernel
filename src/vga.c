#include "utility.h"
#include "error.h"
//extern void *memcpy2(void *,void *,size_t);

#define VGA_BASE 0xb8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define LINE(x) (x/VGA_WIDTH);

static unsigned short *vgaBuff = (unsigned short*)VGA_BASE;
static int width = VGA_WIDTH;
static int height = VGA_HEIGHT;
static int cursor = 80;
//static unsigned char color = FG(VGA_LIGHT_GREY) | BG(VGA_BLACK);
void VGA_display_char(char c)
{
	for(int i=0;i<25*80;i++)
        {
                vgaBuff[cursor++] = (0x2f << 8) | c;
        }
        scroll();
        cursor = 24*80;
        for(int i=0;i<VGA_WIDTH;i++) vgaBuff[cursor++] = (0x1f << 8) | c;
#if 0
	if (c == '\n') {
		cursor = (LINE(cursor) + 1) * width;
		if (cursor >= width*height)
			scroll();
	}
	else if (c == '\r')
		cursor = LINE(cursor);
	else {
		vgaBuff[cursor] = (color << 8) | c;
		if ( (cursor % width) < (width - 1))
			cursor++;
	}
#endif
}

int scroll()
{
        if(!memcpy2(vgaBuff,vgaBuff+VGA_WIDTH,2*24*80))
        {
                return MEMCPY_ERR; 
        }

}
