
#include <ng/basic.h>
#include <stddef.h>
#include <stdint.h>
#include <ng/print.h>
#include <ds/vector.h>
#include <ng/syscall.h>
#include <ds/ringbuf.h>
#include <ng/uart.h>
#include <fs/vfs.h>

ssize_t dev_zero_read(struct fs_node *n, void *data_, size_t len) {
    (void)n;

    char *data = data_;

    for (size_t i=0; i<len; i++) {
        data[i] = 0;
    }
    return len;
}

ssize_t dev_null_write(struct fs_node *n, const void *data, size_t len) {
    (void)n;
    (void)data;
    (void)len;

    return len;
}

ssize_t dev_inc_read(struct fs_node *n, void *data_, size_t len) {
    (void)n;

    char *data = data_;

    for (size_t i=0; i<len; i++) {
        data[i] = i;
    }
    return len;
}

ssize_t serial_write(struct fs_node *n, const void *data_, size_t len) {
    (void)n;
    const char *data = data_;
    uart_write(data, len);
    return len;
}

ssize_t file_buf_read(struct fs_node *n, void *data_, size_t len) {
    char *data = data_;

    size_t count = ring_read(&n->buffer, data, len);

    if (count == 0) {
        return -1;
    }

    return count;
}

