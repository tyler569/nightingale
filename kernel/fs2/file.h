#pragma once
#include "types.h"

#include <fcntl.h>

struct file_operations {
    ssize_t (*read)(struct fs2_file *, char *buffer, size_t len);
    ssize_t (*write)(struct fs2_file *, const char *buffer, size_t len);
};

struct fs2_file {
    struct dentry *dentry;
    struct inode *inode;
    const struct file_operations *ops;
    enum open_flags flags;

    off_t offset;
    void *extra;
};

// Get a fs2_file from the running_process's fd table
struct fs2_file *get_file(int fd);
int add_file(struct fs2_file *fd);


bool read_permission(struct fs2_file *fs2_file);
bool write_permission(struct fs2_file *fs2_file);
bool execute_permission(struct fs2_file *fs2_file);
