
#include <stddef.h>
#include "../llio/portio.h"

#define COM1 (port)0x3f8

struct uart {
    port base;
    void (*write)(char *buf, size_t len);
};

void uart_init();

struct uart com1;

