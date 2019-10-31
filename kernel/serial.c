
#include <ng/basic.h>

#if X86
#include <ng/x86/uart.h>
#endif

void serial_init() {
#if X86
        x86_uart_init(COM1);
#else
#error "unimplemented"
#endif
}

void serial_write(const char c) {
#if X86
        x86_uart_write_byte(COM1, c);
#else
#error "unimplemented"
#endif
}

void serial_write_str(const char *buf, size_t len) {
#if X86
        x86_uart_write(COM1, buf, len);
#else
#error "unimplemented"
#endif
}

char serial_read(const char c) {
#if X86
        return x86_uart_read_byte(COM1);
#else
#error "unimplemented"
#endif
}
