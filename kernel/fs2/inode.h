#pragma once
#include <fcntl.h>
#include <ng/sync.h>
#include "types.h"

struct inode_operations {
    // self operations
    // int (*drop)(struct inode *);
    int (*open)(struct inode *, struct fs2_file *);
    int (*close)(struct inode *, struct fs2_file *);

    // child operations
    int (*create)(struct inode *, struct dentry *, int mode);
    int (*mkdir)(struct inode *, struct dentry *, int mode);
    int (*mknod)(
        struct inode *,
        struct dentry *,
        int mode,
        int minor,
        int major
    );
    int (*remove)(struct inode *, struct dentry *);
    int (*lookup)(struct inode *, struct dentry *);
};

enum inode_flags {
    IS_DIRECTORY = 0x01,
};

struct inode {
    int filesystem_id;
    enum inode_flags flags;
    enum file_type type;
    int inode_number;
    int mode;
    int uid;
    int gid;

    atomic_int dentry_refcnt;

    const struct inode_operations *ops;
    const struct file_operations *file_ops;
    waitqueue_t read_queue;
    waitqueue_t write_queue;

    size_t len;
    void *data;

    void *extra;
    list children; // dentry->children_node
};

// eventually file_system->new_inode();
struct inode *new_inode(int mode);
