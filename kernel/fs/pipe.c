#include <fcntl.h>
#include <ng/common.h>
#include <ng/fs/file.h>
#include <ng/fs/file_system.h>
#include <ng/fs/pipe.h>
#include <ng/fs/vnode.h>
#include <ng/signal.h>
#include <ng/sync.h>
#include <stdlib.h>
#include <string.h>

#define wait_on wq_block_on
#define wake_from wq_notify_all

struct vnode_ops pipe_ops;
struct file_ops pipe_file_ops;

struct vnode *new_pipe() {
	struct vnode *vnode = new_vnode(initfs_file_system, S_IFIFO | 0777);
	vnode->capacity = 16384;
	vnode->data = malloc(vnode->capacity);
	vnode->ops = &pipe_ops;
	vnode->file_ops = &pipe_file_ops;
	vnode->type = FT_PIPE;
	return vnode;
}

int pipe_open(struct vnode *pipe, struct file *file) {
	if (pipe->is_anon_pipe)
		return 0;

	if (file->flags & O_WRONLY && file->flags & O_RDONLY)
		return -EINVAL;

	if (file->flags & O_WRONLY && pipe->read_refcnt == 0)
		wait_on(&pipe->write_queue);

	if (file->flags & O_RDONLY && pipe->write_refcnt == 0)
		wait_on(&pipe->read_queue);

	if (file->flags & O_WRONLY && pipe->write_refcnt == 0)
		wake_from(&pipe->read_queue);

	if (file->flags & O_RDONLY && pipe->read_refcnt == 0)
		wake_from(&pipe->write_queue);

	return 0;
}

int pipe_close(struct vnode *pipe, struct file *file) {
	if (pipe->write_refcnt == 0)
		wake_from(&pipe->read_queue);
	if (pipe->read_refcnt == 0)
		wake_from(&pipe->write_queue);
	return 0;
}

struct vnode_ops pipe_ops = {
	.open = pipe_open,
	.close = pipe_close,
};

ssize_t pipe_read(struct file *file, char *buffer, size_t len) {
	struct vnode *vnode = file->vnode;
	while (vnode->len == 0 && vnode->write_refcnt)
		wait_on(&vnode->read_queue);

	size_t to_read = MIN(len, vnode->len);
	memcpy(buffer, vnode->data, to_read);
	memmove(vnode->data, PTR_ADD(vnode->data, to_read), vnode->len - to_read);
	vnode->len -= to_read;
	wake_from(&vnode->write_queue);
	return to_read;
}

ssize_t pipe_write(struct file *file, const char *buffer, size_t len) {
	struct vnode *vnode = file->vnode;

	if (!vnode->read_refcnt) {
		signal_self(SIGPIPE);
		return 0;
	}

	while (vnode->len == vnode->capacity && vnode->read_refcnt)
		wait_on(&vnode->write_queue);

	if (!vnode->read_refcnt) {
		signal_self(SIGPIPE);
		return 0;
	}

	size_t to_write = MIN(len, vnode->capacity - vnode->len);
	memcpy(PTR_ADD(vnode->data, vnode->len), buffer, to_write);
	vnode->len += to_write;
	wake_from(&vnode->read_queue);
	return to_write;
}

struct file_ops pipe_file_ops = {
	.read = pipe_read,
	.write = pipe_write,
};
