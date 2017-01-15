
/*
 * Basic screen stuff
 * See http://www.osdever.net/bkerndev/Docs/printing.htm
 */

#include <system.h>
#include <screen.h>

vga_char *textmemptr;
int attrib = 0x0F;
int csr_x = 0, csr_y = 0;

void cls() {
    vga_char blank =
        { .fgcolor = COLOR_LIGHT_GREY,
          .bgcolor = COLOR_BLACK,
          .value = ' '
        };
    for (int i=0; i<80*25; i++) {
        textmemptr[i] = blank;
    }
}

void scroll() {
    vga_char blank = 
        { .fgcolor = COLOR_BLACK, 
          .bgcolor = COLOR_BLACK, 
          .value = ' '
        };

    vga_char temp;

}

void move_csr() {
    int temp = csr_y * 80 + csr_x;

	outportb(0x3D4, 14);
	outportb(0x3D5, temp >> 8);
	outportb(0x3D4, 15);
	outportb(0x3D5, temp); 
}

void init_screen() {
    textmemptr = (vga_char *)0xB8000;
    cls();
}
