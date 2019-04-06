
#include <basic.h>
#include <print.h>
#include <panic.h>
#include <thread.h>
#include "cpu.h"
#include "pic.h"
#include "portio.h"
#include "uart.h"

// place in input FD
#include <ringbuf.h>
#include <fs/vfs.h>

#define UART_DATA 0
#define UART_INTERRUPT_ENABLE 1
#define UART_BAUD_LOW 0
#define UART_BAUD_HIGH 1
#define UART_FIFO_CTRL 2
#define UART_LINE_CTRL 3
#define UART_MODEM_CTRL 4
#define UART_LINE_STATUS 5
#define UART_MODEM_STATUS 6


static bool is_transmit_empty(port com) {
    return (inb(com + UART_LINE_STATUS) & 0x20) != 0;
}

static bool is_data_available(port com) {
    return (inb(com + UART_LINE_STATUS) & 0x01) != 0;
}

void x86_uart_write(port p, const char *buf, size_t len) {
    for (size_t i=0; i<len; i++) {
        while (! is_transmit_empty(p)) {}

        switch (buf[i]) {
            case '\n':
                outb(p + UART_DATA, '\r');
                outb(p + UART_DATA, '\n');
                break;
            default:
                outb(p + UART_DATA, buf[i]);
        }
    }
}

char x86_uart_read_byte(port com) {
    while (! is_data_available(com)) {}
    return inb(com + UART_DATA);
}

void x86_uart_enable_interrupt(port com) {
    // For now, I only support interrupt on data available
    outb(com + UART_INTERRUPT_ENABLE, 0x9);
}

void x86_uart_disable_interrupt(port com) {
    outb(com + UART_INTERRUPT_ENABLE, 0x0);
}

// TODO: cleanup with registers above
void x86_uart_init(port p) {
    outb(p + 1, 0x00);
    outb(p + 3, 0x80);
    outb(p + 0, 0x03);
    outb(p + 1, 0x00);
    outb(p + 3, 0x03);
    outb(p + 2, 0xC7);
    outb(p + 4, 0x0B);
}


void x86_uart_irq_handler(struct interrupt_frame *r) {
    char f = x86_uart_read_byte(COM1);

    // serial uses \r, I want \n.
    f = (f == 0x0d) ? 0x0a : f;

    if (f == 0x15) { // ^U
        panic_bt();
    }
   
    // Put that char in the serial device
    struct fs_node *node = dmgr_get(&fs_node_table, 1); // serial device file
    ring_write(&node->buffer, &f, 1);
    pic_send_eoi(r->interrupt_number - 32);

    wake_blocked_threads(&node->blocked_threads);
}

