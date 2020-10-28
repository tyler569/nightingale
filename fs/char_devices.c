
#include <basic.h>
#include <ng/syscall.h>
#include <ng/serial.h>
#include <ng/ringbuf.h>
#include <ng/fs.h>
#include <ng/tty.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

ssize_t dev_zero_read(struct open_file *n, void *data_, size_t len) {
        (void)n;

        char *data = data_;

        for (size_t i = 0; i < len; i++) {
                data[i] = 0;
        }
        return len;
}

ssize_t dev_null_write(struct open_file *n, const void *data, size_t len) {
        (void)n;
        (void)data;
        (void)len;

        return len;
}

