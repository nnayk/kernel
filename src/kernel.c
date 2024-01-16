#include "vga.h"
void kmain()
{
	int loop = 0;
        char *x="Hello";
        VGA_display_str(x);
        //VGA_clear();
	while(!loop)
	{
	}
}
