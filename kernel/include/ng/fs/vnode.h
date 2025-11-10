#pragma once

#include "types.h"
#include <dirent.h>
#include <fcntl.h>
#include <list.h>
#include <ng/sync.h>
#include <ng/time.h>
#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

struct vnode_ops {
	int (*open)(struct vnode *, struct file *);
	int (*close)(struct vnode *, struct file *);

	// struct dentry *(*readlink)(struct vnode *);
	ssize_t (*readlink)(struct vnode *, char *, size_t);
	struct dentry *(*lookup)(struct dentry *, const char *);
};

extern struct vnode_ops default_ops;

enum vnode_flags {
	INODE_UNUSED,
};

struct vnode {
	enum vnode_flags flags;
	enum file_type type;
	int vnode_number;
	struct file_system *file_system;
	int mode;
	int uid;
	int gid;
	int device_major;
	int device_minor;

	// Incremented by attach_vnode
	atomic_int dentry_refcnt;

	// Incremented by open_file
	// Decremented by close_file
	atomic_int read_refcnt;
	atomic_int write_refcnt;

	const struct vnode_ops *ops;
	const struct file_ops *file_ops;
	waitqueue_t read_queue;
	waitqueue_t write_queue;

	time_t atime;
	time_t mtime;
	time_t ctime;

	size_t len;
	size_t capacity;
	void *data;
	void *extra;
	const char *symlink_destination;
	list_node fs_vnodes; // file_system->vnodes

	bool is_anon_pipe;
};

int open_file(struct file *file);
int open_file_clone(struct file *file);
int close_file(struct file *file);
void maybe_delete_vnode(struct vnode *vnode);

static inline void access_vnode(struct vnode *i) {
	i->atime = time_now();
}
static inline void modify_vnode(struct vnode *i) {
	i->mtime = time_now();
}
static inline void change_vnode(struct vnode *i) {
	i->ctime = time_now();
}

END_DECLS
