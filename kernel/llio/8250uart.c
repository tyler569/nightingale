
#include <stddef.h>
#include "portio.h"
#include "8250uart.h"

static void write(char *buf, size_t len) {
    for (size_t i=0; i<len; i++) {
        outb(COM1, buf[i]);
    }
}

void uart_init() {
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
    com1.base = 0x3f8;
    com1.write = &write;
}

