#pragma once

#include "sys/cdefs.h"
#include "types.h"
#include <list.h>

BEGIN_DECLS

extern struct file_system *initfs_file_system;
extern struct file_system *proc_file_system;
extern list mounted_file_systems;
extern struct file_system_operations default_file_system_ops;

struct file_system_operations {
    struct inode *(*new_inode)(struct file_system *);
    struct inode *(*get_inode)(struct file_system *, long);
    void (*destroy_inode)(struct inode *);

    int (*mount)(struct file_system *, struct dentry *);
};

struct file_system_type {
    struct file_system_operations *ops;
};

struct file_system {
    struct file_system_operations *ops;
    struct dentry *root;

    int next_inode_number; // for in-memory filesystems
    list_node node; // mounted_file_systems->
    list_head inodes; // inode->fs_inode
};

// The _notime version of new_inode is intended for file systems that
// save the mtime, atime, and ctime seperately. By default, new_inode
// populates these with the current time, so if that's not needed,
// _notime is an optimization.
struct inode *new_inode_notime(struct file_system *file_system, int mode);
struct inode *new_inode(struct file_system *, int mode);
// void destroy_inode(struct inode *);
// void mount(struct file_system *, struct dentry *);

void mount_file_system(struct file_system *file_system, struct dentry *dentry);

END_DECLS
