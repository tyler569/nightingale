#pragma once

#include "sys/cdefs.h"
#include "types.h"
#include <list.h>

BEGIN_DECLS

extern struct file_system *initfs_file_system;
extern struct file_system *proc_file_system;
extern struct list_head mounted_file_systems;
extern struct file_system_ops default_file_system_ops;

struct file_system_ops {
	struct vnode *(*new_vnode)(struct file_system *);
	struct vnode *(*get_vnode)(struct file_system *, long);
	void (*destroy_vnode)(struct vnode *);

	int (*mount)(struct file_system *, struct dentry *);
};

struct file_system_type {
	struct file_system_ops *ops;
};

struct file_system {
	struct file_system_ops *ops;
	struct dentry *root;

	int next_vnode_number; // for in-memory filesystems
	struct list_head node; // mounted_file_systems->
	struct list_head vnodes; // vnode->fs_vnode
};

// The _notime version of new_vnode is intended for file systems that
// save the mtime, atime, and ctime seperately. By default, new_vnode
// populates these with the current time, so if that's not needed,
// _notime is an optimization.
struct vnode *new_vnode_notime(struct file_system *file_system, int mode);
struct vnode *new_vnode(struct file_system *, int mode);
// void destroy_vnode(struct vnode *);
// void mount(struct file_system *, struct dentry *);

void mount_file_system(struct file_system *file_system, struct dentry *dentry);

END_DECLS
