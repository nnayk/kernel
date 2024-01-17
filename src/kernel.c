#include "vga.h"
void kmain()
{
	int loop = 0;
        char *x="aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
        char *y="He";
        VGA_display_str(x);
        VGA_display_str(y);
        //VGA_clear();
	while(!loop)
	{
	}
}
