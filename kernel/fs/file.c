#include <assert.h>
#include <errno.h>
#include <ng/common.h>
#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/vnode.h>
#include <ng/thread.h>
#include <stdlib.h>
#include <string.h>

// Get a file from the running_process's fd table
struct file *get_file(int fd);
// Get a file from someone else's fd table (?)
struct file *get_file_from_process(int fd, struct process *process);

ssize_t default_read(struct file *file, char *buffer, size_t len) {
	if (file->offset > file->vnode->len)
		return 0;
	size_t to_read = MIN(len, file->vnode->len - file->offset);
	memcpy(buffer, PTR_ADD(file->vnode->data, file->offset), to_read);
	file->offset += to_read;
	return to_read;
}

ssize_t default_write(struct file *file, const char *buffer, size_t len) {
	if (!file->vnode->data) {
		file->vnode->data = malloc(1024);
		file->vnode->capacity = 1024;
	}

	size_t final_len = file->offset + len;
	if (file->offset + len > file->vnode->capacity) {
		size_t resized_len = final_len * 3 / 2;
		file->vnode->data = realloc(file->vnode->data, resized_len);
		file->vnode->capacity = resized_len;
	}

	memcpy(PTR_ADD(file->vnode->data, file->offset), buffer, len);
	file->offset += len;
	file->vnode->len = MAX(file->vnode->len, final_len);
	return len;
}

off_t default_seek(struct file *file, off_t offset, int whence) {
	off_t new_offset = file->offset;

	switch (whence) {
	case SEEK_SET:
		new_offset = offset;
		break;
	case SEEK_CUR:
		new_offset += offset;
		break;
	case SEEK_END:
		new_offset = file->vnode->len + offset;
		break;
	default:
		return -EINVAL;
	}

	if (new_offset < 0)
		return -EINVAL;

	file->offset = new_offset;
	return new_offset;
}

struct file_ops default_file_ops = { 0 };

bool read_mode(struct file *file) { return file->flags & O_RDONLY; }

bool write_mode(struct file *file) { return file->flags & O_WRONLY; }

bool has_permission(struct vnode *vnode, int flags) {
	// bootleg implies, truth table:
	// mode  flags allowed
	// 0     0     1
	// 0     1     0
	// 1     0     1
	// 1     1     1

	return (vnode->mode & USR_READ || !(flags & O_RDONLY))
		&& (vnode->mode & USR_WRITE || !(flags & O_WRONLY));
}

bool write_permission(struct vnode *i) { return !!(i->mode & USR_WRITE); }
bool read_permission(struct vnode *i) { return !!(i->mode & USR_READ); }
bool execute_permission(struct vnode *i) { return !!(i->mode & USR_EXEC); }

struct file *get_file(int fd) {
	if (fd > running_process->n_files || fd < 0) {
		return nullptr;
	}
	if (running_process->files[fd])
		assert(running_process->files[fd]->magic == FILE_MAGIC);

	return running_process->files[fd];
}

static int expand_fds(int new_max) {
	struct file **fds = running_process->files;

	int prev_max = running_process->n_files;
	if (new_max == 0)
		new_max = prev_max * 2;

	struct file **new_memory = zrealloc(fds, new_max * sizeof(struct file *));
	if (!new_memory)
		return -ENOMEM;
	running_process->files = new_memory;
	running_process->n_files = new_max;
	return 0;
}

int add_file(struct file *file) {
	file->magic = FILE_MAGIC;
	struct file **fds = running_process->files;
	int i;
	for (i = 0; i < running_process->n_files; i++) {
		if (!fds[i]) {
			fds[i] = file;
			return i;
		}
	}

	int err;
	if ((err = expand_fds(0)))
		return err;
	running_process->files[i] = file;
	return i;
}

int add_file_at(struct file *file, int at) {
	file->magic = FILE_MAGIC;
	struct file **fds = running_process->files;

	int err;
	if (at < 0)
		return -EBADF;
	if (at >= running_process->n_files && (err = expand_fds(at + 1)))
		return err;

	running_process->files[at] = file;
	return at;
}

struct file *p_remove_file(struct process *proc, int fd) {
	if (fd > proc->n_files || fd < 0)
		return nullptr;

	struct file **fds = proc->files;

	struct file *file = fds[fd];
	if (file)
		assert(file->magic == FILE_MAGIC);

	fds[fd] = 0;
	return file;
}

struct file *remove_file(int fd) { return p_remove_file(running_process, fd); }

void close_all_files(struct process *proc) {
	struct file *file;
	for (int i = 0; i < proc->n_files; i++) {
		if ((file = p_remove_file(proc, i)))
			close_file(file);
	}
	free(proc->files); // ?
}

void close_all_cloexec_files(struct process *proc) {
	struct file *file;
	for (int i = 0; i < proc->n_files; i++) {
		if ((file = proc->files[i]) && (file->flags & O_CLOEXEC)) {
			file = p_remove_file(proc, i);
			close_file(file);
		}
	}
}

struct file *clone_file(struct file *file) {
	assert(file->magic == FILE_MAGIC);
	struct file *new = malloc(sizeof(struct file));
	*new = *file;
	open_file_clone(new);
	return new;
}

struct file **clone_all_files(struct process *proc) {
	struct file **fds = proc->files;
	size_t n_fds = proc->n_files;
	if (n_fds == 0 || n_fds > 1000000) {
		printf("process %i has %li fds\n", proc->pid, n_fds);
		return nullptr;
	}
	struct file **newfds = calloc(n_fds, sizeof(struct file *));
	for (int i = 0; i < n_fds; i++) {
		if (fds[i])
			newfds[i] = clone_file(fds[i]);
	}
	return newfds;
}

ssize_t read_file(struct file *file, char *buffer, size_t len) {
	assert(file->magic == FILE_MAGIC);
	if (!read_mode(file))
		return -EPERM;
	if (file->vnode->type == FT_DIRECTORY)
		return -EISDIR;

	access_vnode(file->vnode);

	if (file->ops->read)
		return file->ops->read(file, buffer, len);
	else
		return default_read(file, buffer, len);
}

ssize_t write_file(struct file *file, const char *buffer, size_t len) {
	assert(file->magic == FILE_MAGIC);
	if (!write_mode(file))
		return -EPERM;
	if (file->vnode->type == FT_DIRECTORY)
		return -EISDIR;

	modify_vnode(file->vnode);

	if (file->ops->write)
		return file->ops->write(file, buffer, len);
	else
		return default_write(file, buffer, len);
}

int ioctl_file(struct file *file, int request, void *argp) {
	assert(file->magic == FILE_MAGIC);
	modify_vnode(file->vnode);

	if (file->ops->ioctl)
		return file->ops->ioctl(file, request, argp);
	else {
		if (request == TTY_ISTTY)
			return 0;
		return -ENOTTY;
	}
}

off_t seek_file(struct file *file, off_t offset, int whence) {
	assert(file->magic == FILE_MAGIC);
	if (file->ops->seek)
		return file->ops->seek(file, offset, whence);
	else
		return default_seek(file, offset, whence);
}

ssize_t getdents_file(struct file *file, struct dirent *buf, size_t len) {
	assert(file->magic == FILE_MAGIC);
	access_vnode(file->vnode);

	if (file->ops->getdents) {
		return file->ops->getdents(file, buf, len);
	} else {
		size_t offset = 0;
		size_t index = 0;
		list_for_each_safe (&file->dentry->children) {
			struct dentry *d = container_of(struct dentry, children_node, it);
			if (index < file->offset) {
				index += 1;
				continue;
			}
			struct dirent *dent = PTR_ADD(buf, offset);
			if (!d->vnode) {
				continue;
			}
			size_t max_copy = MIN(256, len - sizeof(struct dirent) - offset);
			size_t str_len = strlen(d->name);
			size_t will_copy = MIN(str_len, max_copy);
			if (will_copy < str_len)
				break;
			strncpy(dent->d_name, d->name, max_copy);
			dent->d_type = d->vnode->type;
			dent->d_mode = (unsigned short)d->vnode->mode;

			size_t reclen
				= sizeof(struct dirent) - 256 + ROUND_UP(will_copy + 1, 8);
			dent->d_reclen = reclen;
			dent->d_ino = d->vnode->vnode_number;
			dent->d_off = d->vnode->len;

			offset += reclen;
			index += 1;
		}
		file->offset = index;
		return offset;
	}
}

ssize_t readlink_vnode(struct vnode *vnode, char *buffer, size_t len) {
	if (vnode->ops->readlink)
		return vnode->ops->readlink(vnode, buffer, len);
	else if (vnode->symlink_destination) {
		size_t str_len = strlen(vnode->symlink_destination);
		strncpy(buffer, vnode->symlink_destination, len);
		if (str_len > len)
			return -ENAMETOOLONG;
		return str_len;
	} else {
		return -EINVAL;
	}
}
