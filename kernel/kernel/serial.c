
#include <stdint.h>
#include <stddef.h>
#include <kernel/serial.h>

int32_t serial_println(char *buf) {
    size_t i = 0;
    while (buf[i])
        serial_write(buf[i++]);
    serial_write('\r');
    serial_write('\n');
    return i + 2;
}

