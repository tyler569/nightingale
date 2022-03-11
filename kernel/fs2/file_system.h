#pragma once
#include <list.h>
#include "types.h"

extern struct file_system *initfs_file_system;
extern struct file_system *proc_file_system;
extern list mounted_file_systems;
extern struct file_system_operations default_file_system_ops;

struct file_system_operations {
    struct inode *(*new_inode)(struct file_system *);
    void (*destroy_inode)(struct inode *);

    int (*mount)(struct file_system *, struct dentry *);
};

struct file_system_type {
    struct file_system_operations *ops;
};

struct file_system {
    struct file_system_operations *ops;
    struct inode *root_inode;
    struct dentry *mounted_on;
    struct dentry *root_dentry;
    int next_inode_number; // for in-memory filesystems
    list_node node; // mounted_file_systems->
};

struct inode *new_inode(struct file_system *, int mode);
void destroy_inode(struct inode *);
void mount(struct file_system *, struct dentry *);
