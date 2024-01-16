#include "vga.h"
void kmain()
{
	int loop = 0;
        char *x="Hello";
	for(int i=0;i<5;i++) VGA_display_char(x[i]);
        VGA_clear();
	while(!loop)
	{
	}
}
