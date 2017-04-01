
#include <stddef.h>
#include "../llio/portio.h"
#include "8250uart.h"

#define COM1_BASE 0x3f8

#define REGISTER_DATA 0
#define REGISTER_INTERRUPT_ENABLE 1
#define REGISTER_BAUD_LOW 0
#define REGISTER_BAUD_HIGH 1
#define REGISTER_FIFO_CTRL 2
#define REGISTER_LINE_CTRL 3
#define REGISTER_MODEM_CTRL 4
#define REGISTER_LINE_STATUS 5
#define REGISTER_MODEM_STATUS 6

/*
static void set_baud_rate_divisor(int divisor) {
    // set dlab
    // change divisor
    // unset dlab
}

static void set_line_protocol(int data_bits, int stop_bits, int parity) {
    // check values
    // adjust values
    // set line_control register
}

// Do something to change interrupt status
// 1 function or many?

// Other functionality
*/

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

