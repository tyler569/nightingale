
#ifndef _ARCH_I386_VGA_H
#define _ARCH_I386_VGA_H

enum vga_color {
    VGA_COLOR_BLACK             = 0,
    VGA_COLOR_BLUE              = 1,
    VGA_COLOR_GREEN             = 2,
    VGA_COLOR_CYAN              = 3,
    VGA_COLOR_RED               = 4,
    VGA_COLOR_MAGENTA           = 5,
    VGA_COLOR_BROWN             = 6,
    VGA_COLOR_LIGHT_GREY        = 7,
    VGA_COLOR_DARK_GREY         = 8,
    VGA_COLOR_LIGHT_BLUE        = 9,
    VGA_COLOR_LIGHT_GREEN       = 10,
    VGA_COLOR_LIGHT_CYAN        = 11,
    VGA_COLOR_LIGHT_RED         = 12,
    VGA_COLOR_LIGHT_MAGENTA     = 13,
    VGA_COLOR_LIGHT_BROWN       = 14,
    VGA_COLOR_WHITE             = 15,
};

static inline uint8_t vga_color_pack(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_char(char value, uint8_t color) {
    return (uint16_t)value | (uint16_t)color << 8;
}


/*
 * MOVE DELETE
void init_screen();
void cls();
void move_csr();
void putchar(char c);
void putstr(char *c);
void putint32(int32_t num);
void set_text_color(vga_color bg, vga_color fg);
void scroll(int32_t lines);
*/

#endif // _ARCH_I386_VGA_H

