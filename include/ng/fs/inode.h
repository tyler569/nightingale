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

struct inode_operations {
	int (*open)(struct inode *, struct file *);
	int (*close)(struct inode *, struct file *);

	// struct dentry *(*readlink)(struct inode *);
	ssize_t (*readlink)(struct inode *, char *, size_t);
	struct dentry *(*lookup)(struct dentry *, const char *);
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
	// Decremented by close_file
	atomic_int read_refcnt;
	atomic_int write_refcnt;

	const struct inode_operations *ops;
	const struct file_operations *file_ops;
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
	list_node fs_inodes; // file_system->inodes

	bool is_anon_pipe;
};

int open_file(struct file *file);
int open_file_clone(struct file *file);
int close_file(struct file *file);
void maybe_delete_inode(struct inode *inode);

inline void access_inode(struct inode *i) { i->atime = time_now(); }
inline void modify_inode(struct inode *i) { i->mtime = time_now(); }
inline void change_inode(struct inode *i) { i->ctime = time_now(); }

END_DECLS