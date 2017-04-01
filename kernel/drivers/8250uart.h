
#include <stddef.h>
#include "../llio/portio.h"

#define COM1 (port)0x3f8

struct uart {
    port base;
    void (*write)(char *buf, size_t len);
};

void uart_init();

char uart_read_byte(port com);
void uart_enable_interrupt(port com);
void uart_disable_interrupt(port com);

struct uart com1;

