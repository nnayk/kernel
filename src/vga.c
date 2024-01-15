#if 1
#define VGA_BASE 0xb8000
static unsigned short *vgaBuff = (unsigned short*)VGA_BASE;
static int width = 80;
static int height = 20;
static int cursor = 0;
//static unsigned char color = FG(VGA_LIGHT_GREY) | BG(VGA_BLACK);
void VGA_display_char(char c)
{
	vgaBuff[cursor] = (0x2f << 8) | c;
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
#endif
