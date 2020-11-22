#include <basic.h>
#include <stdio.h>

#if X86
#include <ng/x86/uart.h>
#else
#error "unimplementled"
#endif // X86

void serial_init() {
    x86_uart_init();
}

void serial_write(const char c) {
    x86_uart_write_byte(COM1, c);
}

void serial_write_str(const char *buf, size_t len) {
    x86_uart_write(COM1, buf, len);
}

char serial_read(const char c) {
    return x86_uart_read_byte(COM1);
}

void serial2_write(const char c) {
    x86_uart_write_byte(COM2, c);
}

void serial2_write_str(const char *buf, size_t len) {
    x86_uart_write(COM2, buf, len);
}

char serial2_read(const char c) {
    return x86_uart_read_byte(COM2);
}
