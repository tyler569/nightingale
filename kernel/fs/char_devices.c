
#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <print.h>
#include <vector.h>
#include <syscall.h>
#include <ringbuf.h>
#include "vfs.h"

size_t dev_zero_read(struct fs_node *n, void *data_, size_t len) {
    char *data = data_;

    for (size_t i=0; i<len; i++) {
        data[i] = 0;
    }
    return len;
}

size_t dev_null_write(struct fs_node *n, const void *data, size_t len) {
    return len;
}

size_t dev_inc_read(struct fs_node *n, void *data_, size_t len) {
    char *data = data_;

    for (size_t i=0; i<len; i++) {
        data[i] = i;
    }
    return len;
}

size_t stdout_write(struct fs_node *n, const void *data_, size_t len) {
    const char *data = data_;

    for (size_t i=0; i<len; i++) {
        printf("%c", data[i]);
    }
    return len;
}

size_t file_buf_read(struct fs_node *n, void *data_, size_t len) {
    char *data = data_;

    size_t count = ring_read(&n->buffer, data, len);

    if (count == 0) {
        return -1;
    }

    return count;
}

