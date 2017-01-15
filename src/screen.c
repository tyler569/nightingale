
/*
 * Basic screen stuff
 * See http://www.osdever.net/bkerndev/Docs/printing.htm
 */

#include <system.h>
#include <screen.h>

vga_char *textmemptr;
int attrib = 0x0F;
int csr_x = 0, csr_y = 0;

void scroll() {
    vga_char blank, temp;

    blank = { .fgcolor = COLOR_BLACK, .bgcolor = COLOR_BLACK, .value = ' ' };


}
