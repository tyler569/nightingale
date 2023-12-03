#include "ng/fs/char_dev.h"
#include "ng/common.h"
#include "ng/fs/file.h"
#include "ng/fs/inode.h"
#include "ng/random.h"
#include <errno.h>
#include <string.h>

ssize_t basic_char_dev_read(struct file *file, char *buffer, size_t len)
{
    size_t i;
    switch (file->inode->device_minor) {
    case FS_DEV_NULL:
        return 0;
    case FS_DEV_ZERO:
        memset(buffer, 0, len);
        return len;
    case FS_DEV_RANDOM:
        return get_random(buffer, len);
    case FS_DEV_INC:
        for (i = 0; i < len / sizeof(unsigned); i++) {
            ((unsigned *)buffer)[i] = i;
        }
        return i * sizeof(unsigned);
    default:
        return -ENODEV;
    }
}

ssize_t basic_char_dev_write(struct file *file, const char *buffer, size_t len)
{
    switch (file->inode->device_minor) {
    case FS_DEV_NULL:
        return len;
    case FS_DEV_ZERO:
        return len;
    case FS_DEV_RANDOM:
        add_to_random(buffer, len);
        return len;
    case FS_DEV_INC:
        return len;
    default:
        return -ENODEV;
    }
}

struct file_operations basic_char_dev_ops = {
    .read = basic_char_dev_read,
    .write = basic_char_dev_write,
};

extern struct file_operations tty_ops;

struct file_operations *char_drivers[256] = {
    [0] = &basic_char_dev_ops,
    [1] = &tty_ops,
};
