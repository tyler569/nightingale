
#include <ng/basic.h>
#include <ng/uart.h>
#include <arch/x86/uart.h>

void uart_init() {
    x86_uart_init(COM1);
}

void uart_write(const char* buf, size_t len) {
    x86_uart_write(COM1, buf, len);
}

char uart_read_byte() {
    return x86_uart_read_byte(COM1);
}

