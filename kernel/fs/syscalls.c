#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <ng/fs/char_dev.h>
#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/file_system.h>
#include <ng/fs/inet_socket.h>
#include <ng/fs/pipe.h>
#include <ng/fs/socket.h>
#include <ng/fs/vnode.h>
#include <ng/netfilter.h>
#include <ng/thread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>

// associate vnode with NEGATIVE dentry
struct file *create_file(struct dentry *dentry, struct vnode *vnode, int flags);
// open existing vnode
struct file *new_file(struct dentry *dentry, int flags);
// Create a file for an vnode that has NO dentry (i.e. pipe)
struct file *no_d_file(struct vnode *vnode, int flags);
// truncate file
void truncate(struct file *file);
// set to append, move cursor
void append(struct file *file);

sysret do_open(struct dentry *cwd, const char *path, int flags, int mode) {
	struct dentry *dentry = resolve_path_from(cwd, path, true);
	if (IS_ERROR(dentry))
		return ERROR(dentry);
	struct vnode *vnode = dentry_vnode(dentry);
	if (!vnode && !(flags & O_CREAT))
		return -ENOENT;
	if (dentry && vnode && flags & O_CREAT && flags & O_EXCL)
		return -EEXIST;

	struct file_system *file_system = dentry_file_system(dentry);
	struct file *file;

	if (vnode && !has_permission(vnode, flags))
		return -EPERM;
	else if (!vnode
		&& !has_permission(dentry_vnode(dentry->parent), flags | O_WRONLY))
		return -EPERM;

	if (!vnode && flags & O_CREAT) {
		vnode = new_vnode(file_system, mode);
		file = create_file(dentry, vnode, flags);
	} else {
		file = new_file(dentry, flags);
	}

	if (flags & O_TRUNC)
		truncate(file);

	if (flags & O_APPEND)
		append(file);

	return add_file(file);
}

sysret sys_openat(int fd, const char *path, int flags, int mode) {
	struct dentry *root = resolve_atfd(fd);

	if (IS_ERROR(root))
		return ERROR(root);

	return do_open(root, path, flags, mode);
}

sysret do_touch(struct dentry *cwd, const char *path, int flags, int mode) {
	struct dentry *dentry = resolve_path_from(cwd, path, true);
	if (IS_ERROR(dentry))
		return ERROR(dentry);
	struct vnode *vnode = dentry_vnode(dentry);

	assert(flags & O_CREAT && flags & O_EXCL);

	if (dentry && vnode)
		return -EEXIST;

	struct file_system *file_system = dentry_file_system(dentry);

	if (!has_permission(dentry_vnode(dentry->parent), flags | O_WRONLY))
		return -EPERM;

	vnode = new_vnode(file_system, mode);
	attach_vnode(dentry, vnode);
	return 0;
}

sysret sys_mkdirat(int fd, const char *path, int mode) {
	struct dentry *root = resolve_atfd(fd);

	if (IS_ERROR(root))
		return ERROR(root);

	return do_touch(root, path, O_CREAT | O_EXCL, _NG_DIR | mode);
}

sysret sys_close(int fd) {
	struct file *file = remove_file(fd);
	if (!file)
		return -EBADF;
	close_file(file);
	return 0;
}

sysret sys_getdents(int fd, struct dirent *buf, size_t len) {
	struct file *directory = get_file(fd);
	if (!directory)
		return -EBADF;
	struct vnode *vnode = directory->vnode;
	if (!vnode)
		return -ENOENT;
	if (vnode->type != FT_DIRECTORY)
		return -ENOTDIR;
	if (!execute_permission(vnode))
		return -EPERM;

	access_vnode(vnode);

	return getdents_file(directory, buf, len);
}

sysret sys_pathname(int fd, char *buffer, size_t len) {
	struct file *file = get_file(fd);
	if (!file)
		return -EBADF;
	struct dentry *dentry = file->dentry;
	if (!dentry)
		return -EINVAL;
	return pathname(dentry, buffer, len);
}

sysret sys_getcwd(char *buffer, size_t len) {
	struct dentry *dentry = running_thread->cwd;
	if (!dentry)
		return -EINVAL;
	return pathname(dentry, buffer, len);
}

sysret sys_chdirat(int atfd, const char *path) {
	struct dentry *dentry = resolve_atpath(atfd, path, true);
	if (IS_ERROR(dentry))
		return ERROR(dentry);
	if (!dentry_vnode(dentry))
		return -ENOENT;

	// FIXME: reference count this!
	running_thread->cwd = dentry;
	return 0;
}

sysret sys_read(int fd, char *buffer, size_t len) {
	struct file *file = get_file(fd);
	if (!file)
		return -EBADF;

	return read_file(file, buffer, len);
}

sysret sys_write(int fd, char *buffer, size_t len) {
	struct file *file = get_file(fd);
	if (!file)
		return -EBADF;

	return write_file(file, buffer, len);
}

sysret sys_ioctl(int fd, int request, void *argp) {
	struct file *file = get_file(fd);
	if (!file)
		return -EBADF;

	return ioctl_file(file, request, argp);
}

sysret sys_lseek(int fd, off_t offset, int whence) {
	struct file *file = get_file(fd);
	if (!file)
		return -EBADF;

	return seek_file(file, offset, whence);
}

sysret stat_vnode(struct vnode *vnode, struct stat *stat) {
	*stat = (struct stat) {
		.st_dev = (dev_t)(intptr_t)vnode->file_system,
		.st_ino = vnode->vnode_number,
		.st_mode = (vnode->mode & 0xFFFF) + (vnode->type << 16),
		.st_nlink = vnode->dentry_refcnt,
		.st_uid = vnode->uid,
		.st_gid = vnode->gid,
		.st_rdev = (vnode->device_major << 16) + vnode->device_minor,
		.st_size = vnode->len,
		.st_blksize = 1024, // FIXME
		.st_blocks = ROUND_UP(vnode->len, 1024) / 1024, // FIXME
		.st_atime = vnode->atime,
		.st_mtime = vnode->mtime,
		.st_ctime = vnode->ctime,
	};
	return 0;
}

sysret sys_fstat(int fd, struct stat *stat) {
	struct file *file = get_file(fd);
	if (!file)
		return -EBADF;

	struct vnode *vnode = file->vnode;
	return stat_vnode(vnode, stat);
}

sysret sys_statat(int atfd, const char *path, struct stat *stat) {
	struct dentry *dentry = resolve_atpath(atfd, path, false);
	if (IS_ERROR(dentry))
		return ERROR(dentry);

	struct vnode *vnode = dentry_vnode(dentry);
	if (!vnode)
		return -ENOENT;
	return stat_vnode(vnode, stat);
}

sysret sys_linkat(
	int oldfdat, const char *oldpath, int newfdat, const char *newpath) {
	struct dentry *olddentry = resolve_atpath(oldfdat, oldpath, true);
	if (IS_ERROR(olddentry))
		return ERROR(olddentry);

	if (!dentry_vnode(olddentry))
		return -ENOENT;

	if (dentry_vnode(olddentry)->type == FT_DIRECTORY)
		return -EISDIR;

	struct dentry *newdentry = resolve_atpath(newfdat, newpath, true);
	if (IS_ERROR(newdentry))
		return ERROR(newdentry);

	if (dentry_vnode(newdentry))
		return -EEXIST;

	if (newdentry->file_system != olddentry->file_system)
		return -EXDEV;

	attach_vnode(newdentry, olddentry->vnode);
	return 0;
}

sysret sys_symlinkat(const char *topath, int newfdat, const char *newpath) {
	struct dentry *dentry = resolve_atpath(newfdat, newpath, true);
	if (IS_ERROR(dentry))
		return ERROR(dentry);

	if (dentry_vnode(dentry))
		return -EEXIST;

	struct vnode *vnode = new_vnode(dentry->file_system, _NG_SYMLINK | 0777);
	vnode->symlink_destination = strdup(topath);
	attach_vnode(dentry, vnode);
	return 0;
}

sysret sys_readlinkat(int atfd, const char *path, char *buffer, size_t len) {
	struct dentry *dentry = resolve_atpath(atfd, path, false);
	if (IS_ERROR(dentry))
		return ERROR(dentry);
	struct vnode *vnode = dentry_vnode(dentry);
	if (!vnode)
		return -ENOENT;
	if (vnode->type != FT_SYMLINK)
		return -EINVAL;

	readlink_vnode(vnode, buffer, len);

	// These can be dynamically generated, check if we don't need it
	// anymore
	maybe_delete_dentry(dentry);
	// return strnlen(buffer, len);
	return strlen(buffer);
}

sysret sys_mknodat(int atfd, const char *path, mode_t mode, dev_t device) {
	struct dentry *dentry = resolve_atpath(atfd, path, true);
	if (IS_ERROR(dentry))
		return ERROR(dentry);
	if (dentry_vnode(dentry))
		return -EEXIST;

	struct file_system *file_system = dentry_file_system(dentry);
	struct vnode *vnode = new_vnode(file_system, mode);
	int device_major = device >> 16;
	int device_minor = device & 0xFFFF;
	struct file_ops *drv_ops = char_drivers[device_major];
	if (!drv_ops)
		return -ENODEV;

	vnode->device_major = device_major;
	vnode->device_minor = device_minor;
	vnode->file_ops = drv_ops;
	vnode->type = FT_CHAR_DEV;

	attach_vnode(dentry, vnode);
	return 0;
}

sysret sys_pipe(int pipefds[static 2]) {
	struct vnode *pipe = new_pipe();
	pipe->is_anon_pipe = true;
	struct file *read_end = no_d_file(pipe, O_RDONLY);
	struct file *write_end = no_d_file(pipe, O_WRONLY);
	pipefds[0] = add_file(read_end);
	pipefds[1] = add_file(write_end);
	return 0;
}

sysret sys_mkpipeat(int atfd, const char *path, mode_t mode) {
	struct dentry *dentry = resolve_atpath(atfd, path, true);
	if (IS_ERROR(dentry))
		return ERROR(dentry);
	if (dentry_vnode(dentry))
		return -EEXIST;

	struct vnode *pipe = new_pipe();
	pipe->is_anon_pipe = false;
	pipe->mode = mode;

	attach_vnode(dentry, pipe);
	return 0;
}

sysret sys_mountat(
	int atfd, const char *target, int type, int s_atfd, const char *source) {
	struct dentry *tdentry = resolve_atpath(atfd, target, true);
	if (IS_ERROR(tdentry))
		return ERROR(tdentry);
	struct vnode *tvnode = dentry_vnode(tdentry);
	if (!tvnode)
		return -ENOENT;
	if (tvnode->type != FT_DIRECTORY)
		return -ENOTDIR;

	struct file_system *file_system;
#define _FS_PROCFS 1

	switch (type) {
	case _FS_PROCFS:
		tdentry->mounted_file_system = proc_file_system;
		break;
	default:
		return -ETODO;
	}

	return 0;
}

sysret sys_dup(int fd) {
	struct file *old = get_file(fd);
	if (!old)
		return -EBADF;
	struct file *new = clone_file(old);
	return add_file(new);
}

sysret sys_dup2(int fd, int newfd) {
	struct file *old = get_file(fd);
	if (!old)
		return -EBADF;
	struct file *new = clone_file(old);

	struct file *close = get_file(newfd);
	if (close)
		close_file(close);
	return add_file_at(new, newfd);
}

sysret sys_fchmod(int fd, int mode) {
	struct file *file = get_file(fd);
	if (!file)
		return -EBADF;
	if (!write_mode(file))
		return -EPERM;
	struct vnode *vnode = file->vnode;
	vnode->mode = (vnode->mode & ~0xFFFF) | (mode & 0xFFFF);
	return 0;
}

sysret sys_chmodat(int atfd, const char *path, int mode) {
	struct dentry *dentry = resolve_atpath(atfd, path, true);
	if (IS_ERROR(dentry))
		return ERROR(dentry);
	struct vnode *vnode = dentry_vnode(dentry);
	if (!vnode)
		return -ENOENT;

	vnode->mode = (vnode->mode & ~0xFFFF) | (mode & 0xFFFF);
	return 0;
}

sysret sys_unlinkat(int atfd, const char *path, int mode) {
	struct dentry *dentry = resolve_atpath(atfd, path, true);
	if (IS_ERROR(dentry))
		return ERROR(dentry);
	struct vnode *vnode = dentry_vnode(dentry);
	if (!vnode)
		return -ENOENT;
	if (!write_permission(vnode))
		return -EPERM;
	if (vnode->type == FT_DIRECTORY)
		return -EISDIR;

	unlink_dentry(dentry);
	detach_vnode(dentry);
	free(dentry);
	return 0;
}

// Given a POSITIVE dentry (extant vnode), create a `struct file` to
// open it and return the new "file" object.
struct file *new_file(struct dentry *dentry, int flags) {
	int err;
	struct file *file = malloc(sizeof(struct file));
	struct vnode *vnode = dentry_vnode(dentry);
	*file = (struct file) {
		.vnode = vnode,
		.dentry = dentry,
		.flags = flags, // validate?
		.ops = vnode->file_ops,
	};

	err = open_file(file);

	if (IS_ERROR(err)) {
		free(file);
		return TO_ERROR(err);
	}

	return file;
}

// Given a NEGATIVE dentry and an vnode, associate the vnode with the
// dentry, then open the file and return it.
struct file *create_file(
	struct dentry *dentry, struct vnode *vnode, int flags) {
	attach_vnode(dentry, vnode);
	return new_file(dentry, flags);
}

// Create a file for an vnode that has NO dentry (i.e. pipe)
struct file *no_d_file(struct vnode *vnode, int flags) {
	int err;
	struct file *file = malloc(sizeof(struct file));
	*file = (struct file) {
		.vnode = vnode,
		.flags = flags, // validate?
		.ops = vnode->file_ops,
	};

	err = open_file(file);

	if (IS_ERROR(err)) {
		free(file);
		return TO_ERROR(err);
	}

	return file;
}

// truncate file
void truncate(struct file *file) { file->vnode->len = 0; }

// set to append, move cursor
void append(struct file *file) { file->offset = file->vnode->len; }

// Socket syscalls
sysret sys_socket(int domain, int type, int protocol) {
	struct vnode *vnode;

	switch (domain) {
	case AF_UNIX:
		vnode = new_socket();
		break;
	case AF_INET:
		if (type != SOCK_DGRAM && type != SOCK_STREAM) {
			return -EPROTONOSUPPORT;
		}
		vnode = new_inet_socket(type);
		break;
	default:
		return -EAFNOSUPPORT;
	}

	if (!vnode) {
		return -ENOMEM;
	}

	struct file *file = no_d_file(vnode, O_RDWR);
	if (IS_ERROR(file)) {
		return ERROR(file);
	}

	return add_file(file);
}

sysret sys_bind(int sockfd, struct sockaddr const *addr, socklen_t addrlen) {
	struct file *file = get_file(sockfd);
	if (!file) {
		return -EBADF;
	}

	struct vnode *vnode = file->vnode;
	if (vnode->type != _NG_SOCK || !vnode->socket_ops
		|| !vnode->socket_ops->bind) {
		return -ENOTSOCK;
	}

	return vnode->socket_ops->bind(vnode, (struct sockaddr *)addr, addrlen);
}

sysret sys_listen(int sockfd, int backlog) {
	struct file *file = get_file(sockfd);
	if (!file) {
		return -EBADF;
	}

	struct vnode *vnode = file->vnode;
	if (vnode->type != _NG_SOCK || !vnode->socket_ops
		|| !vnode->socket_ops->listen) {
		return -ENOTSOCK;
	}

	return vnode->socket_ops->listen(vnode, backlog);
}

sysret sys_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	struct file *file = get_file(sockfd);
	if (!file) {
		return -EBADF;
	}

	struct vnode *vnode = file->vnode;
	if (vnode->type != _NG_SOCK || !vnode->socket_ops
		|| !vnode->socket_ops->accept) {
		return -ENOTSOCK;
	}

	return vnode->socket_ops->accept(vnode, addr, addrlen);
}

sysret sys_connect(int sockfd, struct sockaddr const *addr, socklen_t addrlen) {
	struct file *file = get_file(sockfd);
	if (!file) {
		return -EBADF;
	}

	struct vnode *vnode = file->vnode;
	if (vnode->type != _NG_SOCK || !vnode->socket_ops
		|| !vnode->socket_ops->connect) {
		return -ENOTSOCK;
	}

	return vnode->socket_ops->connect(vnode, (struct sockaddr *)addr, addrlen);
}

sysret sys_send(int sockfd, void const *buf, size_t len, int flags) {
	struct file *file = get_file(sockfd);
	if (!file) {
		return -EBADF;
	}

	struct vnode *vnode = file->vnode;
	if (vnode->type != _NG_SOCK || !vnode->socket_ops
		|| !vnode->socket_ops->send) {
		return -ENOTSOCK;
	}

	return vnode->socket_ops->send(vnode, buf, len, flags);
}

sysret sys_recv(int sockfd, void *buf, size_t len, int flags) {
	struct file *file = get_file(sockfd);
	if (!file) {
		return -EBADF;
	}

	struct vnode *vnode = file->vnode;
	if (vnode->type != _NG_SOCK || !vnode->socket_ops
		|| !vnode->socket_ops->recv) {
		return -ENOTSOCK;
	}

	return vnode->socket_ops->recv(vnode, buf, len, flags);
}

sysret sys_sendto(int sockfd, void const *buf, size_t len, int flags,
	struct sockaddr const *dest_addr, socklen_t addrlen) {
	struct file *file = get_file(sockfd);
	if (!file) {
		return -EBADF;
	}

	struct vnode *vnode = file->vnode;
	if (vnode->type != _NG_SOCK) {
		return -ENOTSOCK;
	}

	// For inet sockets, use sendto implementation
	if (vnode->data) {
		// This is a bit of a hack - we should have a better way to identify
		// inet sockets
		extern int inet_sendto(struct vnode * vnode, const void *buf,
			size_t len, int flags, struct sockaddr *dest_addr,
			socklen_t addrlen);
		return inet_sendto(
			vnode, buf, len, flags, (struct sockaddr *)dest_addr, addrlen);
	}

	return -ENOTSOCK;
}

sysret sys_recvfrom(int sockfd, void *buf, size_t len, int flags,
	struct sockaddr *src_addr, socklen_t *addrlen) {
	struct file *file = get_file(sockfd);
	if (!file) {
		return -EBADF;
	}

	struct vnode *vnode = file->vnode;
	if (vnode->type != _NG_SOCK || !vnode->socket_ops
		|| !vnode->socket_ops->recv) {
		return -ENOTSOCK;
	}

	// For now, just call recv and ignore the address information
	// TODO: Implement proper recvfrom with address extraction
	return vnode->socket_ops->recv(vnode, buf, len, flags);
}

// Netfilter management syscalls
sysret sys_netfilter_add_rule(int hook, const struct net_rule *rule) {
	if (hook < 0 || hook >= NF_INET_NUMHOOKS) {
		return -EINVAL;
	}

	if (!rule) {
		return -EFAULT;
	}

	// Copy rule from user space
	struct net_rule *kernel_rule = malloc(sizeof(struct net_rule));
	if (!kernel_rule) {
		return -ENOMEM;
	}

	memcpy(kernel_rule, rule, sizeof(struct net_rule));
	kernel_rule->next = nullptr; // Will be set by nf_add_rule

	int result = nf_add_rule(hook, kernel_rule);
	if (result < 0) {
		free(kernel_rule);
		return result;
	}

	printf("Added netfilter rule '%s' to hook %d\n", kernel_rule->name, hook);
	return 0;
}

sysret sys_netfilter_remove_rule(int hook, const char *name) {
	if (hook < 0 || hook >= NF_INET_NUMHOOKS) {
		return -EINVAL;
	}

	if (!name) {
		return -EFAULT;
	}

	// Copy name from user space (simple approach - assume it's accessible)
	int result = nf_remove_rule(hook, name);
	if (result < 0) {
		return -ENOENT; // Rule not found
	}

	printf("Removed netfilter rule '%s' from hook %d\n", name, hook);
	return 0;
}

struct nf_stats {
	uint64_t hook_counts[NF_INET_NUMHOOKS];
	uint64_t accept_count;
	uint64_t drop_count;
};

sysret sys_netfilter_get_stats(void *stats_buf) {
	if (!stats_buf) {
		return -EFAULT;
	}

	// For now, just print stats instead of copying to user space
	// TODO: Implement proper user space memory copying
	nf_print_stats();
	return 0;
}
