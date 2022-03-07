#include <basic.h>
#include <errno.h>
#include <string.h>
#include "../random.h"
#include "char_dev.h"
#include "file.h"
#include "inode.h"

ssize_t basic_char_dev_read(struct fs2_file *file, char *buffer, size_t len) {
    size_t i;
    switch (file->inode->device_minor) {
    case FS2_DEV_NULL:
        return 0;
    case FS2_DEV_ZERO:
        memset(buffer, 0, len);
        return len;
    case FS2_DEV_RANDOM:
        return get_random(buffer, len);
    case FS2_DEV_INC:
        for (i = 0; i < len / sizeof(unsigned); i++) {
            ((unsigned *)buffer)[i] = i;
        }
        return i * sizeof(unsigned);
    default:
        return -ENODEV;
    }
}

ssize_t basic_char_dev_write(struct fs2_file *file, const char *buffer, size_t len) {
    switch (file->inode->device_minor) {
    case FS2_DEV_NULL:
        return len;
    case FS2_DEV_ZERO:
        return len;
    case FS2_DEV_RANDOM:
        add_to_random(buffer, len);
        return len;
    case FS2_DEV_INC:
        return len;
    default:
        return -ENODEV;
    }
}

struct file_operations basic_char_dev_ops = {
    .read = basic_char_dev_read,
    .write = basic_char_dev_write,
};

struct file_operations *char_drivers[256] = {
    [0] = &basic_char_dev_ops,
};
