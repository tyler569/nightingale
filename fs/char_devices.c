#include <basic.h>
#include <ng/fs.h>

ssize_t dev_zero_read(struct open_file *n, void *data_, size_t len) {
        char *data = data_;

        for (size_t i = 0; i < len; i++) {
                data[i] = 0;
        }
        return len;
}

ssize_t dev_null_write(struct open_file *n, const void *data, size_t len) {
        return len;
}

ssize_t dev_null_read(struct open_file *n, void *data, size_t len) {
        return 0;
}
