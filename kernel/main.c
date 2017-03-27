
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "multiboot2.h"
#include "video/terminal.h"
#include "llio/8250uart.h"

void dbg_fmt_ptr(char *buf, uintptr_t ptr) {
    int i = 0;
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
    printf("Hello World\n");


    printf("  %%i: %i %i %i\n", 0, 1234, -1);
    printf("  %%u: %u %u %u\n", 0, 1234, -1);
    printf("  %%x: %x %x %x\n", 0, 1234, -1);
    
    com1.write("Hello World\r\n", 13);

    int j = 0;
    return 1/j;
}

