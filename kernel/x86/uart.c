
#include <ng/basic.h>
#include <ng/panic.h>
#include <ng/print.h>
#include <ng/thread.h>
#include <ng/tty.h>
#include <ng/x86/uart.h>
#include <ng/x86/cpu.h>
#include <ng/x86/pic.h>
#include <ng/x86/portio.h>

/*
// place in input FD
#include <ng/ringbuf.h>
#include <ng/fs.h>
*/

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
        for (size_t i = 0; i < len; i++) {
                while (!is_transmit_empty(p)) {
                }

                switch (buf[i]) {
                case '\r':
                        // FIXME: this problem is solved by a line discipline
                        // (or whatever I would call it, that's Linux's
                        // name) - see kernel/tty.c
                        break;
                case '\n':
                        outb(p + UART_DATA, '\r');
                        outb(p + UART_DATA, '\n');
                        break;
                default:
                        outb(p + UART_DATA, buf[i]);
                }
        }
}

void x86_uart_write_byte(port p, const char b) {
        while (!is_transmit_empty(p)) {
        }
        outb(p + UART_DATA, b);
}

char x86_uart_read_byte(port com) {
        while (!is_data_available(com)) {
        }
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

        x86_uart_enable_interrupt(p);
}

void x86_uart_irq4_handler(struct interrupt_frame *r) {
        char f = x86_uart_read_byte(COM1);

        write_to_serial_tty(&serial_tty, f);
        pic_send_eoi(r->interrupt_number - 32);
}

void x86_uart_irq3_handler(struct interrupt_frame *r) {
        char f = x86_uart_read_byte(COM2);

        write_to_serial_tty(&serial_tty2, f);
        pic_send_eoi(r->interrupt_number - 32);
}

