#pragma once

#include "file_system.h"
#include "sys/cdefs.h"
#include "types.h"
#include <list.h>
#include <stdatomic.h>

BEGIN_DECLS

struct dentry_ops {
	char unused;
};

enum dentry_flags {
	UNUSED,
};

struct dentry {
	const struct dentry_ops *ops;
	struct vnode *vnode;
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

inline struct vnode *dentry_vnode(struct dentry *dentry) {
	if (dentry->mounted_file_system) {
		return dentry->mounted_file_system->root->vnode;
	} else {
		return dentry->vnode;
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
	struct dentry *dentry, const char *name, struct vnode *vnode);
struct dentry *find_child(struct dentry *, const char *);
struct dentry *unlink_dentry(struct dentry *dentry);
int attach_vnode(struct dentry *, struct vnode *);
void detach_vnode(struct dentry *);
void maybe_delete_dentry(struct dentry *);

struct dentry *resolve_path(const char *path);
struct dentry *resolve_atfd(int fd);
struct dentry *resolve_atpath(int fd, const char *path, bool follow);
struct dentry *resolve_path_from(
	struct dentry *cursor, const char *path, bool follow);
int pathname(struct dentry *dentry, char *buffer, size_t len);

END_DECLS
