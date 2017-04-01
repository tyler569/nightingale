
#include <stdbool.h>
#include <stddef.h>
#include "../llio/portio.h"
#include "8250uart.h"

#define COM1_BASE 0x3f8

#define DATA 0
#define INTERRUPT_ENABLE 1
#define BAUD_LOW 0
#define BAUD_HIGH 1
#define FIFO_CTRL 2
#define LINE_CTRL 3
#define MODEM_CTRL 4
#define LINE_STATUS 5
#define MODEM_STATUS 6

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

static bool is_transmit_empty(port com) {
    return (inb(com + LINE_STATUS) & 0x20) != 0;
}

static bool is_data_available(port com) {
    return (inb(com + LINE_STATUS) & 0x01) != 0;
}

static void write(char *buf, size_t len) {
    for (size_t i=0; i<len; i++) {
        while (! is_transmit_empty(COM1)) {}
        outb(COM1 + DATA, buf[i]);
    }
}

char uart_read_byte(port com) {
    while (! is_data_available(com)) {}
    return inb(com + DATA);
}

void uart_enable_interrupt(port com) {
    // For now, I only support interrupt on data available
    outb(com + INTERRUPT_ENABLE, 0x9);
}

void uart_disable_interrupt(port com) {
    outb(com + INTERRUPT_ENABLE, 0x0);
}

// TODO: cleanup with registers above
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

