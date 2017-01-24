
#include <stdint.h>
#include <stdio.h>

#include <kernel/serial.h>
#include <kernel/printk.h>

static loglevel kernel_loglevel = LOG_INFO;

int32_t printk(const char *format) { // TODO: VA_ARGS
    loglevel l;

    if (format[0] == '\x01') {
        l = format[1];
        format += 2;
    } else
        l = LOG_INFO;

    if (kernel_loglevel <= loglevel) {
        printf("%s\n", buf);
        serial_println(buf);
    }

    return 0;

}


