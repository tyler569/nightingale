
#include <stddef.h>
#include <stdint.h>

#include "multiboot2.h"

void dbg_print_str(size_t offset, char color, char *string) {
    short *vga = (short *)0xB8000 + offset;
    int i = 0;
    while (string[i] != 0) {
        vga[i] = string[i] | (color << 8);
        i++;
    }
}

void dbg_print_ptr(size_t offset, char color, uintptr_t ptr) {
    short *vga = (short *)0xB8000 + offset;
    int i = 0;
    for (int bit = 60; bit >= 0; bit -= 4) {
        char value = (ptr >> bit) & 0xF;
        if (value >= 0xA) {
            vga[i] = (value - 0xA + 'A') | (color << 8);
        } else {
            vga[i] = (value + '0') | (color << 8);
        }
        i++;
    }
}
        

int main(int mb, multiboot_info_t *mbinfo);
    dbg_print_str(80, 0x4f, "This is 64 bit C");
    dbg_print_ptr(160, 0x1f, (uintptr_t)x);
    dbg_print_ptr(240, 0x1f, (uintptr_t)y);
    dbg_print_ptr(320, 0x1f, (uintptr_t)z);
    dbg_print_ptr(400, 0x1f, (uintptr_t)a1);
    dbg_print_ptr(480, 0x1f, (uintptr_t)a2);
    dbg_print_ptr(560, 0x1f, (uintptr_t)a3);

    return 0;
}

