#pragma once
#include "types.h"

struct inode_operations {
    // self operations
    // int (*drop)(struct inode *);
    int (*open)(struct inode *, struct file *);
    int (*close)(struct inode *, struct file *);

    // child operations
    int (*create)(struct inode *, struct dentry *, int mode);
    int (*mkdir)(struct inode *, struct dentry *, int mode);
    int (*mknod)(struct inode *, struct dentry *, int mode, int minor, int major);
    int (*remove)(struct inode *, struct dentry *);
    int (*lookup)(struct inode *, struct dentry *);
};

struct inode {
    int filesystem_id;
    int mode;
    int uid;
    int gid;
    const struct inode_operations *ops;
    const struct file_operations *file_ops;
    waitqueue_t read_queue;
    waitqueue_t write_queue;

    void *extra;
    list children; // dentry->children_node
};

// eventually file_system->new_inode();
struct inode *new_inode(void);
