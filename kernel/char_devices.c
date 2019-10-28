
#include <ng/basic.h>
#include <ng/print.h>
#include <ng/syscall.h>
#include <ng/serial.h>
#include <ds/ringbuf.h>
#include <ds/vector.h>
#include <ng/fs.h>
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

ssize_t dev_serial_write(struct open_fd *n, const void *data_, size_t len) {
        (void)n;
        const char *data = data_;
        serial_write_str(data, len);
        return len;
}

ssize_t dev_serial_read(struct open_fd *n, void *data_, size_t len) {
        char *data = data_;

        size_t count = ring_read(&n->node->extra.ring, data, len);

        if (count == 0) {
                return -1;
        }

        return count;
}

