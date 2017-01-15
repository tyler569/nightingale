
/*
 * Basic screen stuff
 * See http://www.osdever.net/bkerndev/Docs/printing.htm
 */

#include <system.h>
#include <screen.h>

vga_char *textmemptr;
vga_color bgcolor = COLOR_BLACK;
vga_color fgcolor = COLOR_LIGHT_GREY;
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
    csr_x = 0;
    csr_y = 0;
    move_csr();
}

void scroll() {
    vga_char blank = 
        { .fgcolor = COLOR_LIGHT_GREY,
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

void putchar(char c) {
    vga_char *where = textmemptr + csr_y * 80 + csr_x;

    if (c == '\n') {
        csr_x = 0;
        csr_y++;
    } else if (c >= ' ') {
        where->value = c;
        where->fgcolor = fgcolor;
        where->bgcolor = bgcolor;
    } else {
        where->value = '?';
    }
    csr_x++;
    if (csr_x >= 80) {
        csr_x = 0;
        csr_y++;
    }
    move_csr();
}

void putstr(char *str) {
    for (int i=0; i<strlen(str); i++) {
        putchar(str[i]);
    }
}

void set_text_color(vga_color bg, vga_color fg) {
    bgcolor = bg;
    fgcolor = fg;
}

void init_screen() {
    textmemptr = (vga_char *)0xB8000;
    cls();
}
