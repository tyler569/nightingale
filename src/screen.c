
/*
 * Basic screen stuff
 * See http://www.osdever.net/bkerndev/Docs/printing.htm
 * for inspiration
 */

#include <system.h>
#include <screen.h>

vga_char *textmemptr;
vga_color bgcolor = COLOR_BLACK;
vga_color fgcolor = COLOR_LIGHT_GREY;
size_t csr_x = 0, csr_y = 0;

const vga_char blank =
    { .fgcolor = COLOR_LIGHT_GREY,
      .bgcolor = COLOR_BLACK,
      .value = ' '
    };

void cls() { 
    for (size_t i=0; i<80*25; i++) {
        textmemptr[i] = blank;
    }
    csr_x = 0;
    csr_y = 0;
    move_csr();
}

void scroll(int32_t lines) {
    memcpy((void *)0xB8000, (void *)0xB8000 + (lines * 80 * sizeof(vga_char)), 80 * 24 * sizeof(vga_char));

    vga_char *clearptr = (vga_char *)0xB8000 + (25 - lines) * 80;
    for (size_t i=0; i < 80*lines; i++) {
        clearptr[i] = blank;
    }
    csr_y -= lines;
    move_csr();
}

void move_csr() {
    uint32_t temp = csr_y * 80 + csr_x;

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
        csr_x++;
    } else {
        where->value = '?';
        csr_x++;
    }
    if (csr_x >= 80) {
        csr_x = 0;
        csr_y++;
    }
    if (csr_y >= 25) {
        scroll(1);
    }
    move_csr();
}

void putstr(char *str) {
    for (int i=0; i<strlen(str); i++) {
        putchar(str[i]);
    }
}

void putint32(int32_t num) {
    char value[10] = "\0\0\0\0\0\0\0\0\0\0";
    size_t position = 0;
    if (num < 0) {
        value[position++] = '-';
        num = -num;
    }
    if (num == 0) {
        value[position++] = '0';
        putstr(value);
        return;
    }
    bool printing = false;
    int32_t power10;
    for (int32_t i = 9; i >= 0; i--) {
        power10 = power(10, i);
        if (num / power10 > 0 || printing) {
            value[position++] = '0' + (num / power10);
            num -= (num / power10) * power10;
            printing = true;
        }
    }
    putstr(value);
}

void set_text_color(vga_color bg, vga_color fg) {
    bgcolor = bg;
    fgcolor = fg;
}

void init_screen() {
    textmemptr = (vga_char *)0xB8000;
    cls();
}

