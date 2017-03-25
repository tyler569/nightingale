
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "multiboot2.h"
#include "video/terminal.h"
#include "llio/8250uart.h"

void dbg_fmt_ptr(char *buf, uintptr_t ptr) {
    int i;
    for (int bit = 60; bit >= 0; bit -= 4) {
        char value = (ptr >> bit) & 0xF;
        if (value >= 0xA) {
            buf[i] = (value - 0xA + 'A');
        } else {
            buf[i] = (value + '0');
        }
        i++;
    }
    buf[i] = '\n';
}


int main(int mb, uintptr_t mb_info) {
    term_init();
    uart_init();

    printf("Hello World\n");
    com1.write("Hello World\r\n", 13);

    return 0;
}

