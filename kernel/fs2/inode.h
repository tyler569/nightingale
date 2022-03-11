#pragma once
#include <fcntl.h>
#include <list.h>
#include <ng/sync.h>
#include <sys/types.h>
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
        dev_t device,
        int mode
    );
    int (*remove)(struct inode *, struct dentry *);
    int (*lookup)(struct inode *, struct dentry *);
};

extern struct inode_operations default_ops;

enum inode_flags {
    INODE_UNUSED,
};

struct inode {
    enum inode_flags flags;
    enum file_type type;
    int inode_number;
    struct file_system *file_system;
    int mode;
    int uid;
    int gid;
    int device_major;
    int device_minor;

    // Incremented by attach_inode
    atomic_int dentry_refcnt;

    // Incremented by open_file
    atomic_int read_refcnt;
    atomic_int write_refcnt;

    const struct inode_operations *ops;
    const struct file_operations *file_ops;
    waitqueue_t read_queue;
    waitqueue_t write_queue;

    long atime;
    long mtime;
    long ctime;

    size_t len;
    size_t capacity;
    void *data;

    void *extra;

    const char *symlink_destination;

    list_node fs_inodes; // file_system->inodes
};

int open_file(struct fs2_file *file);
int close_file(struct fs2_file *file);
