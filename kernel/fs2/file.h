#pragma once
#include "types.h"

#include <fcntl.h>

// enum fs2_file_flags {
//     O_RDONLY = 0x01,
//     O_WRONLY = 0x02,
//     O_RDWR = O_RDONLY | O_WRONLY,
//     O_APPEND = 0x04,
//     O_ASYNC = 0x08,
//     O_CLOEXEC = 0x10,
//     O_NONBLOCK = 0x20,
//     O_NDELAY = O_NONBLOCK,
// };

struct file_operations {
    ssize_t (*read)(struct file *, char *buffer, size_t len);
    ssize_t (*write)(struct file *, const char *buffer, size_t len);
};

struct fs2_file {
    struct dentry *dentry;
    struct file_operations *ops;
    enum file_flags flags;

    off_t offset;
    void *extra;
};

// Get a file from the running_process's fd table
struct file *get_file(int fd);
int add_file(struct file *fd);
