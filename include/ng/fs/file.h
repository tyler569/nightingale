#pragma once

#include "types.h"
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

struct file_ops {
	ssize_t (*read)(struct file *, char *buffer, size_t len);
	ssize_t (*write)(struct file *, const char *buffer, size_t len);
	int (*ioctl)(struct file *, int request, void *argp);
	off_t (*seek)(struct file *, off_t offset, int whence);
	ssize_t (*getdents)(struct file *, struct dirent *, size_t);
};

extern struct file_ops default_file_ops;

struct file {
#define FILE_MAGIC 47841728
	long magic;
	struct dentry *dentry;
	struct vnode *vnode;
	const struct file_ops *ops;
	enum open_flags flags;

	off_t offset;
	void *extra;
	size_t len;
	size_t size;
};

// Get a file from the running_process's fd table
struct file *get_file(int fd);
int add_file(struct file *fd);
int add_file_at(struct file *fd, int at);
struct file *remove_file(int fd);

ssize_t default_read(struct file *, char *, size_t);
ssize_t default_write(struct file *, const char *, size_t);

bool read_mode(struct file *file);
bool write_mode(struct file *file);
bool has_permission(struct vnode *vnode, int flags);
bool read_permission(struct vnode *vnode);
bool write_permission(struct vnode *vnode);
bool execute_permission(struct vnode *vnode);

ssize_t read_file(struct file *file, char *buffer, size_t len);
ssize_t write_file(struct file *file, const char *buffer, size_t len);
int ioctl_file(struct file *file, int request, void *argp);
off_t seek_file(struct file *file, off_t offset, int whence);
ssize_t getdents_file(struct file *file, struct dirent *buf, size_t len);
ssize_t readlink_vnode(struct vnode *vnode, char *buffer, size_t len);

struct process;
struct file *clone_file(struct file *file);
struct file **clone_all_files(struct process *proc);
void close_all_files(struct process *proc);
void close_all_cloexec_files(struct process *proc);

END_DECLS
