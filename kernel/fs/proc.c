#include <fcntl.h>
#include <ng/common.h>
#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/proc.h>
#include <ng/fs/vnode.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern struct file_system *proc_file_system;
struct file_ops proc_file_ops;
struct vnode_ops proc_vnode_ops;

struct vnode *new_proc_vnode(
	int mode, void (*generate)(struct file *, void *arg), void *arg) {
	struct vnode *vnode = new_vnode(proc_file_system, _NG_PROC | mode);
	vnode->ops = &proc_vnode_ops;
	vnode->file_ops = &proc_file_ops;
	vnode->extra = generate;
	vnode->data = arg;
	return vnode;
}

void make_proc_file(
	const char *name, void (*generate)(struct file *, void *arg), void *arg) {
	struct dentry *root = proc_file_system->root;
	struct vnode *vnode = new_proc_vnode(0444, generate, arg);
	struct dentry *dentry = resolve_path_from(root, name, true);
	if (dentry->vnode) {
		printf("proc file '%s' already exists\n", name);
		maybe_delete_vnode(vnode);
		return;
	}
	attach_vnode(dentry, vnode);
}

ssize_t proc_file_read(struct file *file, char *buffer, size_t len) {
	size_t to_read = MIN(file->len - file->offset, len);
	memcpy(buffer, PTR_ADD(file->extra, file->offset), to_read);
	file->offset += to_read;
	return to_read;
}

ssize_t proc_file_write(struct file *file, const char *buffer, size_t len) {
	return -ETODO;
}

off_t proc_file_seek(struct file *file, off_t offset, int whence) {
	off_t new_offset = file->offset;

	switch (whence) {
	case SEEK_SET:
		new_offset = offset;
		break;
	case SEEK_CUR:
		new_offset += offset;
		break;
	case SEEK_END:
		// The only difference between this and default_seek is that
		// this uses file->len for SEEK_END because the vnode has no
		// len of its own.
		new_offset = file->len + offset;
		break;
	default:
		return -EINVAL;
	}

	if (new_offset < 0)
		return -EINVAL;

	file->offset = new_offset;
	return new_offset;
}

int proc_file_open(struct vnode *vnode, struct file *file) {
	void (*generate)(struct file *, void *arg);
	file->extra = malloc(4096 * 4);
	file->size = 4096 * 4;
	file->len = 0;
	generate = vnode->extra;
	generate(file, vnode->data);
	return 0;
}

int proc_file_close(struct vnode *vnode, struct file *file) {
	if (file->extra)
		free(file->extra);
	return 0;
}

struct file_ops proc_file_ops = {
	.read = proc_file_read,
	.write = proc_file_write,
	.seek = proc_file_seek,
};

struct vnode_ops proc_vnode_ops = {
	.open = proc_file_open,
	.close = proc_file_close,
};

void proc_sprintf(struct file *file, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	file->len += vsnprintf(
		file->extra + file->len, file->size - file->len, fmt, args);
	// va_end is called in vsnprintf
}
