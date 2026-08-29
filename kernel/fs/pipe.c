#include <fcntl.h>
#include <ng/fs/file.h>
#include <ng/fs/file_system.h>
#include <ng/fs/pipe.h>
#include <ng/fs/vnode.h>
#include <ng/signal.h>
#include <ng/sync.h>
#include <stdlib.h>
#include <string.h>

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

	mutex_lock(&pipe->guard);

	while (file->flags & O_WRONLY && pipe->read_refcnt == 0)
		cv_wait(&pipe->write_queue, &pipe->guard);

	while (file->flags & O_RDONLY && pipe->write_refcnt == 0)
		cv_wait(&pipe->read_queue, &pipe->guard);

	if (file->flags & O_WRONLY && pipe->write_refcnt == 0)
		cv_wake_all_locked(&pipe->read_queue);

	if (file->flags & O_RDONLY && pipe->read_refcnt == 0)
		cv_wake_all_locked(&pipe->write_queue);

	mutex_unlock(&pipe->guard);

	return 0;
}

int pipe_close(struct vnode *pipe, struct file *file) {
	mutex_lock(&pipe->guard);

	if (pipe->write_refcnt == 0)
		cv_wake_all_locked(&pipe->read_queue);
	if (pipe->read_refcnt == 0)
		cv_wake_all_locked(&pipe->write_queue);

	mutex_unlock(&pipe->guard);

	return 0;
}

struct vnode_ops pipe_ops = {
	.open = pipe_open,
	.close = pipe_close,
};

static bool pipe_can_read(struct vnode *v) {
	return v->len > 0 || v->write_refcnt == 0;
}

static size_t pipe_copy_to_user(
	struct vnode *v, char *user_buffer, size_t len) {
	size_t to_read = MIN(len, v->len);
	memcpy(user_buffer, v->data, to_read);
	memmove(v->data, v->data + to_read, v->len - to_read);
	v->len -= to_read;
	cv_wake_all_locked(&v->write_queue);
	return to_read;
}

static size_t pipe_copy_from_user(
	struct vnode *v, const char *user_buffer, size_t len) {
	size_t to_write = MIN(len, v->capacity - v->len);
	memcpy(v->data + v->len, user_buffer, to_write);
	v->len += to_write;
	cv_wake_all_locked(&v->read_queue);
	return to_write;
}

ssize_t pipe_read(struct file *file, char *buffer, size_t len) {
	struct vnode *v = file->vnode;

	mutex_lock(&v->guard);

	while (!pipe_can_read(v))
		cv_wait(&v->read_queue, &v->guard);

	ssize_t out = pipe_copy_to_user(v, buffer, len);

	mutex_unlock(&v->guard);

	return out;
}

ssize_t pipe_write(struct file *file, const char *buffer, size_t len) {
	struct vnode *v = file->vnode;

	mutex_lock(&v->guard);

	while (v->len == v->capacity && v->read_refcnt > 0)
		cv_wait(&v->write_queue, &v->guard);

	if (v->read_refcnt == 0) {
		mutex_unlock(&v->guard);
		signal_self(SIGPIPE);
		return 0;
	}

	ssize_t out = pipe_copy_from_user(v, buffer, len);

	mutex_unlock(&v->guard);

	return out;
}

struct file_ops pipe_file_ops = {
	.read = pipe_read,
	.write = pipe_write,
};
