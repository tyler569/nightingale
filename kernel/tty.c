
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/cpu.h>
#include <kernel/tty.h>
#include <kernel/vga.h>


static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

void terminal_initialize() {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_color_pack(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_char(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(uint8_t c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_char(c, color);
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_row += 1;
        terminal_column = 0;
    } else if (c == '\t') {
        terminal_write("        ", (TAB_WIDTH - terminal_row % TAB_WIDTH));
    } else if (c == '\r') {
        terminal_column = 0;
    } else {
        if (c < ' ') 
            terminal_putentryat((uint8_t)'?', terminal_color, terminal_column, terminal_row);
        else
            terminal_putentryat((uint8_t)c, terminal_color, terminal_column, terminal_row);
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            terminal_row += 1;
        }
    }
    if (terminal_row > VGA_HEIGHT - 1)
        terminal_scroll();
    terminal_cursor_update(terminal_row, terminal_column);
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

void terminal_cursor_update(size_t row, size_t column) {
    size_t index = row * VGA_WIDTH + column;
    outportb(0x3D4, 14);
    outportb(0x3D5, index >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, index);
}

void terminal_scroll() {
    memmove(
        VGA_MEMORY, VGA_MEMORY + VGA_WIDTH,
        VGA_WIDTH * (VGA_HEIGHT - 1) * 2
    );
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        (VGA_MEMORY + (24 * VGA_WIDTH))[x] = vga_char(' ', terminal_color);
    }
    terminal_row -= 1;
}

