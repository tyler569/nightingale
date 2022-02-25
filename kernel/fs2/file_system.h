#pragma once
#include "types.h"

struct file_system_operations {
    struct inode *(*alloc_inode)(void);
    void (*dealloc_inode)(struct inode *);

    int (*mount)(struct file_system *, struct dentry *);
};

struct file_system_type {
    struct file_system_operations *ops;
};

struct file_system {
    struct file_system_operations *ops;
    struct inode *root_inode;
};
