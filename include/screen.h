
#ifndef _SCREEN_H
#define _SCREEN_H

typedef enum _vga_color {
    COLOR_BLACK             = 0,
    COLOR_BLUE              = 1,
    COLOR_GREEN             = 2,
    COLOR_CYAN              = 3,
    COLOR_RED               = 4,
    COLOR_MAGENTA           = 5,
    COLOR_BROWN             = 6,
    COLOR_LIGHT_GREY        = 7,
    COLOR_DARK_GREY         = 8,
    COLOR_LIGHT_BLUE        = 9,
    COLOR_LIGHT_GREEN       = 10,
    COLOR_LIGHT_CYAN        = 11,
    COLOR_LIGHT_RED         = 12,
    COLOR_LIGHT_MAGENTA     = 13,
    COLOR_LIGHT_BROWN       = 14,
    COLOR_WHITE             = 15,
} vga_color;

typedef struct __attribute__((packed)) _vga_char {
    char value;
    vga_color fgcolor :4;
    vga_color bgcolor :4;
} vga_char;

void init_screen();
void cls();
void move_csr();
void putchar(char c);
void putstr(char *c);
void putint(int num);
void set_text_color(vga_color bg, vga_color fg);
void scroll();



#endif // _SCREEN_H
