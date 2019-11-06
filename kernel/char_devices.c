
#include <basic.h>
#include <ng/print.h>
#include <ng/syscall.h>
#include <ng/serial.h>
#include <ng/ringbuf.h>
#include <ng/vector.h>
#include <ng/fs.h>
#include <ng/tty.h>
#include <stddef.h>
#include <stdint.h>

ssize_t dev_zero_read(struct open_fd *n, void *data_, size_t len) {
        (void)n;

        char *data = data_;

        for (size_t i = 0; i < len; i++) {
                data[i] = 0;
        }
        return len;
}

ssize_t dev_null_write(struct open_fd *n, const void *data, size_t len) {
        (void)n;
        (void)data;
        (void)len;

        return len;
}

ssize_t dev_serial_write(struct open_fd *n, const void *data, size_t len) {
        n->node->tty->print_fn(data, len);
        return len;
}

ssize_t dev_serial_read(struct open_fd *n, void *data_, size_t len) {
        char *data = data_;

        ssize_t count = ring_read(&n->node->ring, data, len);

        if (count == 0) {
                return -1;
        }

        return count;
}

