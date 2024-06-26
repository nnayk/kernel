// header files
#include "vga.h"
#include "utility.h"
#include "memutils.h"
#include "error.h"
#include "irq.h"

//macros
#define VGA_BASE 0xb8000 // starting address of VGA console (top left corner)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_CHAR_SIZE 2 // bytes
#define VGA_DIMS VGA_WIDTH * VGA_HEIGHT
#define BOTTOM_LEFT (VGA_HEIGHT-1)*VGA_WIDTH
#define BOTTOM_RIGHT VGA_DIMS-1
#define LINE(x) ((x)/VGA_WIDTH) 

//function prototypes
int scroll();

// global vars
static unsigned short *vgaBuff = (unsigned short*)VGA_BASE;
static int width = VGA_WIDTH;
static int height = VGA_HEIGHT;
static int cursor = 0;
static unsigned char color = 0x2f;
//static unsigned char color = FG(VGA_LIGHT_GREY) | BG(VGA_BLACK);

int  VGA_display_str(const char *s)
{
        //int loop = 0;
        if(!s) return ERR_NULL_PTR;
        size_t len = strlen(s);
        int last_char_line = 0;
        if(len <= VGA_WIDTH && LINE(cursor) 
                       != (last_char_line = LINE(cursor + len - 1)))
                cursor = last_char_line * VGA_WIDTH;
        for(int i=0;i<len;i++)
        {
                VGA_display_char(s[i]);
        }
        return SUCCESS;
}

int VGA_clear()
{
        if(!memset(vgaBuff,0,VGA_DIMS*VGA_CHAR_SIZE)) 
                return ERR_MEMCPY; 
        return SUCCESS;
}

void VGA_display_char(char c)
{
    int int_set = 0;
    if(are_interrupts_enabled())
    {
       int_set = 1;
       cli();
    }
    if(cursor == BOTTOM_RIGHT+1)
    {
       scroll();
       cursor=BOTTOM_LEFT;
    }
	//int errCode = 0;
	if (c == '\n') {
		cursor = (LINE(cursor) + 1) * width;
		if (cursor >= width*height)
                {
                        scroll();
                        cursor = BOTTOM_LEFT;
                }
	}
	else if (c == '\r')
		cursor = LINE(cursor)*VGA_WIDTH;
	else {
		vgaBuff[cursor] = (color << 8) | c;
		//if ((cursor % width) < (width - 1))
                cursor++;
	}
    if(int_set) sti();
}

int scroll()
{
        if(!memcpy(vgaBuff,vgaBuff+VGA_WIDTH,VGA_CHAR_SIZE*BOTTOM_LEFT)) 
                return ERR_MEMCPY;
        memset(vgaBuff+BOTTOM_LEFT,0,VGA_WIDTH*VGA_CHAR_SIZE);
        return SUCCESS;
}

int VGA_row_count()
{
        return VGA_HEIGHT;
}

int VGA_col_count()
{
        return VGA_WIDTH;
}

void VGA_display_attr_char(int x, int y, char c, int fg, int bg)
{
        if(!((0 < x) && (x < VGA_WIDTH)) || !((0 < y) && (y < VGA_HEIGHT))) 
                        return; 
        int temp_cursor = VGA_WIDTH*y + x;
        int color = (bg << 12) | (fg << 8);
        unsigned short final_char = color | c;
        vgaBuff[temp_cursor] = final_char;
}
