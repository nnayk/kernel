// header files
#include "utility.h"
#include "error.h"

//macros
#define VGA_BASE 0xb8000 // starting address of VGA console (top left corner)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_CHAR_SIZE 2 // bytes
#define VGA_DIMS VGA_WIDTH * VGA_HEIGHT
#define BOTTOM_LEFT (VGA_HEIGHT-1)*VGA_WIDTH
#define LINE(x) (x/VGA_WIDTH) 

//function prototypes
int scroll();

// global vars
static unsigned short *vgaBuff = (unsigned short*)VGA_BASE;
static int width = VGA_WIDTH;
static int height = VGA_HEIGHT;
static int cursor = 0;
static unsigned char color = 0x2f;
//static unsigned char color = FG(VGA_LIGHT_GREY) | BG(VGA_BLACK);

int VGA_clear()
{
        if(!memset2(vgaBuff,0,VGA_DIMS)) 
                return MEMCPY_ERR; 
        return SUCCESS;
        
}

void VGA_display_char(char c)
{
	int errCode = 0;
	if (c == '\n') {
		cursor = (LINE(cursor) + 1) * width;
		if (cursor >= width*height)
                {
			scroll();
                        cursor = BOTTOM_LEFT;
                }
	}
	else if (c == '\r')
		cursor = LINE(cursor);
	else {
		vgaBuff[cursor] = (color << 8) | c;
		//if ((cursor % width) < (width - 1))
			cursor++;
	}
}

int scroll()
{
        if(!memcpy2(vgaBuff,vgaBuff+VGA_WIDTH,VGA_CHAR_SIZE*BOTTOM_LEFT)) 
                return MEMCPY_ERR; 
        return SUCCESS;
}

