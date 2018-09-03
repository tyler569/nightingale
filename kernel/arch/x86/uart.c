
#include <basic.h>
#include "interrupt.h"
#include "pic.h"
#include <print.h>
#include "portio.h"
#include "uart.h"

// place in input FD
#include <ringbuf.h>
#include <fs/vfs.h>

#define DATA 0
#define INTERRUPT_ENABLE 1
#define BAUD_LOW 0
#define BAUD_HIGH 1
#define FIFO_CTRL 2
#define LINE_CTRL 3
#define MODEM_CTRL 4
#define LINE_STATUS 5
#define MODEM_STATUS 6


static bool is_transmit_empty(port com) {
    return (inb(com + LINE_STATUS) & 0x20) != 0;
}

static bool is_data_available(port com) {
    return (inb(com + LINE_STATUS) & 0x01) != 0;
}

void uart_write(port p, const char *buf, size_t len) {
    for (size_t i=0; i<len; i++) {
        while (! is_transmit_empty(p)) {}

        switch (buf[i]) {
            case '\n':
                outb(p + DATA, '\r');
                outb(p + DATA, '\n');
                break;
            default:
                outb(p + DATA, buf[i]);
        }
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
void uart_init(port p) {
    outb(p + 1, 0x00);
    outb(p + 3, 0x80);
    outb(p + 0, 0x03);
    outb(p + 1, 0x00);
    outb(p + 3, 0x03);
    outb(p + 2, 0xC7);
    outb(p + 4, 0x0B);
}


void uart_irq_handler(struct interrupt_frame *r) {
    char f = uart_read_byte(COM1);
    // uart_write(COM1, &f, 1);
#if 0
    if (f == 0x0d /* \r */) {
        printf("[0] received: %02x (%3i / '\\r')\n", f, f);
    } else {
        printf("[0] received: %02x (%3i / '%c')\n", f, f, f);
    }
#endif

    // serial uses \r, I want \n.
    f = (f == 0x0d) ? 0x0a : f;
   
    // Put that char in the serial device
    struct fs_node *node = vec_get(fs_node_table, 4); // serial device file
    ring_write(&node->buffer, &f, 1);

    pic_send_eoi(r->interrupt_number - 32);
}

