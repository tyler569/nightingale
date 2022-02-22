#include <basic.h>
#include <ng/fs.h>
#include <stdio.h>
#include "../random.h"

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

ssize_t dev_random_read(struct open_file *n, void *data, size_t len) {
    return get_random(data, len);
}

ssize_t dev_random_write(struct open_file *n, const void *data, size_t len) {
    add_to_random(data, len);
    return len;
}

ssize_t dev_count_read(struct open_file *n, void *data, size_t len) {
    int *int_data = data;
    for (size_t n = 0; n < len/sizeof(int); n++) {
        int_data[n] = n;
    }
    return len;
}
