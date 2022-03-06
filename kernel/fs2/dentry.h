#pragma once

#include <list.h>
#include <stdatomic.h>
#include "types.h"
#include "file_system.h"

struct dentry_operations {
    char unused;
};

enum dentry_flags {
    UNUSED,
};

struct dentry {
    const struct dentry_operations *ops;
    struct inode *inode;
    struct dentry *parent;
    const char *name;
    enum dentry_flags flags;
    struct file_system *file_system;
    struct file_system *mounted_file_system;

    list children; // ->children_node
    list_node children_node;

    // Incremented by open_file
    // Decremented by close_file
    atomic_int file_refcnt;
};

extern struct dentry *global_root_dentry;

inline struct inode *dentry_inode(struct dentry *dentry) {
    if (dentry->mounted_file_system) {
        return dentry->mounted_file_system->root_inode;
    } else {
        return dentry->inode;
    }
}

inline struct file_system *dentry_file_system(struct dentry *dentry) {
    if (dentry->mounted_file_system) {
        return dentry->mounted_file_system;
    } else {
        return dentry->file_system;
    }
}

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
