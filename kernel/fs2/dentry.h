#pragma once

#include <list.h>
#include <stdatomic.h>
#include "types.h"

struct dentry_operations {
    char c;
};

enum dentry_flags {
    DENTRY_IS_MOUNTPOINT = 0x01,
};

struct dentry {
    // struct dentry_operations *ops;
    struct inode *inode;
    struct dentry *parent;
    const char *name;
    enum dentry_flags flags;
    struct file_system *file_system;

    list children; // ->children_node
    list_node children_node;

    atomic_int file_refcnt;
};

extern struct dentry *global_root_dentry;

struct inode *dentry_inode(struct dentry *dentry);
struct dentry *new_dentry();

struct dentry *add_child(
    struct dentry *dentry,
    const char *name,
    struct inode *inode
);
struct dentry *find_child(struct dentry *, const char *);

struct dentry *resolve_path(const char *path);
struct dentry *resolve_atfd(int fd);
struct dentry *resolve_path_from(struct dentry *cursor, const char *path);
int pathname(struct fs2_file *fs2_file, char *buffer, size_t len);
