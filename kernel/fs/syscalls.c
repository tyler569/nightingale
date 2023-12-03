#include "ng/fs/char_dev.h"
#include "ng/fs/dentry.h"
#include "ng/fs/file.h"
#include "ng/fs/file_system.h"
#include "ng/fs/inode.h"
#include "ng/fs/pipe.h"
#include "ng/string.h"
#include "ng/thread.h"
#include <assert.h>
#include <basic.h>
#include <dirent.h>
#include <fcntl.h>
#include <list.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// associate inode with NEGATIVE dentry
struct file *create_file(struct dentry *dentry, struct inode *inode, int flags);
// open existing inode
struct file *new_file(struct dentry *dentry, int flags);
// Create a file for an inode that has NO dentry (i.e. pipe)
struct file *no_d_file(struct inode *inode, int flags);
// truncate file
void truncate(struct file *file);
// set to append, move cursor
void append(struct file *file);

sysret do_open(struct dentry *cwd, const char *path, int flags, int mode)
{
    struct dentry *dentry = resolve_path_from(cwd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    struct inode *inode = dentry_inode(dentry);
    if (!inode && !(flags & O_CREAT))
        return -ENOENT;
    if (dentry && inode && flags & O_CREAT && flags & O_EXCL)
        return -EEXIST;

    struct file_system *file_system = dentry_file_system(dentry);
    struct file *file;

    if (inode && !has_permission(inode, flags))
        return -EPERM;
    else if (!inode
        && !has_permission(dentry_inode(dentry->parent), flags | O_WRONLY))
        return -EPERM;

    if (!inode && flags & O_CREAT) {
        inode = new_inode(file_system, mode);
        file = create_file(dentry, inode, flags);
    } else {
        file = new_file(dentry, flags);
    }

    if (flags & O_TRUNC)
        truncate(file);

    if (flags & O_APPEND)
        append(file);

    return add_file(file);
}

sysret sys_openat(int fd, const char *path, int flags, int mode)
{
    struct dentry *root = resolve_atfd(fd);

    if (IS_ERROR(root))
        return ERROR(root);

    return do_open(root, path, flags, mode);
}

sysret do_touch(struct dentry *cwd, const char *path, int flags, int mode)
{
    struct dentry *dentry = resolve_path_from(cwd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    struct inode *inode = dentry_inode(dentry);

    assert(flags & O_CREAT && flags & O_EXCL);

    if (dentry && inode)
        return -EEXIST;

    struct file_system *file_system = dentry_file_system(dentry);

    if (!has_permission(dentry_inode(dentry->parent), flags | O_WRONLY))
        return -EPERM;

    inode = new_inode(file_system, mode);
    attach_inode(dentry, inode);
    return 0;
}

sysret sys_mkdirat(int fd, const char *path, int mode)
{
    struct dentry *root = resolve_atfd(fd);

    if (IS_ERROR(root))
        return ERROR(root);

    return do_touch(root, path, O_CREAT | O_EXCL, _NG_DIR | mode);
}

sysret sys_close(int fd)
{
    struct file *file = remove_file(fd);
    if (!file)
        return -EBADF;
    close_file(file);
    return 0;
}

sysret sys_getdents(int fd, struct dirent *buf, size_t len)
{
    struct file *directory = get_file(fd);
    if (!directory)
        return -EBADF;
    struct inode *inode = directory->inode;
    if (!inode)
        return -ENOENT;
    if (inode->type != FT_DIRECTORY)
        return -ENOTDIR;
    if (!execute_permission(inode))
        return -EPERM;

    access_inode(inode);

    return getdents_file(directory, buf, len);
}

sysret sys_pathname(int fd, char *buffer, size_t len)
{
    struct file *file = get_file(fd);
    if (!file)
        return -EBADF;
    struct dentry *dentry = file->dentry;
    if (!dentry)
        return -EINVAL;
    return pathname(dentry, buffer, len);
}

sysret sys_getcwd(char *buffer, size_t len)
{
    struct dentry *dentry = running_thread->cwd;
    if (!dentry)
        return -EINVAL;
    return pathname(dentry, buffer, len);
}

sysret sys_chdirat(int atfd, const char *path)
{
    struct dentry *dentry = resolve_atpath(atfd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    if (!dentry_inode(dentry))
        return -ENOENT;

    // FIXME: reference count this!
    running_thread->cwd = dentry;
    return 0;
}

sysret sys_read(int fd, char *buffer, size_t len)
{
    struct file *file = get_file(fd);
    if (!file)
        return -EBADF;

    return read_file(file, buffer, len);
}

sysret sys_write(int fd, char *buffer, size_t len)
{
    struct file *file = get_file(fd);
    if (!file)
        return -EBADF;

    return write_file(file, buffer, len);
}

sysret sys_ioctl(int fd, int request, void *argp)
{
    struct file *file = get_file(fd);
    if (!file)
        return -EBADF;

    return ioctl_file(file, request, argp);
}

sysret sys_lseek(int fd, off_t offset, int whence)
{
    struct file *file = get_file(fd);
    if (!file)
        return -EBADF;

    return seek_file(file, offset, whence);
}

sysret stat_inode(struct inode *inode, struct stat *stat)
{
    *stat = (struct stat) {
        .st_dev = (dev_t)(intptr_t)inode->file_system,
        .st_ino = inode->inode_number,
        .st_mode = (inode->mode & 0xFFFF) + (inode->type << 16),
        .st_nlink = inode->dentry_refcnt,
        .st_uid = inode->uid,
        .st_gid = inode->gid,
        .st_rdev = (inode->device_major << 16) + inode->device_minor,
        .st_size = inode->len,
        .st_blksize = 1024, // FIXME
        .st_blocks = round_up(inode->len, 1024) / 1024, // FIXME
        .st_atime = inode->atime,
        .st_mtime = inode->mtime,
        .st_ctime = inode->ctime,
    };
    return 0;
}

sysret sys_fstat(int fd, struct stat *stat)
{
    struct file *file = get_file(fd);
    if (!file)
        return -EBADF;

    struct inode *inode = file->inode;
    return stat_inode(inode, stat);
}

sysret sys_statat(int atfd, const char *path, struct stat *stat)
{
    struct dentry *dentry = resolve_atpath(atfd, path, false);
    if (IS_ERROR(dentry))
        return ERROR(dentry);

    struct inode *inode = dentry_inode(dentry);
    if (!inode)
        return -ENOENT;
    return stat_inode(inode, stat);
}

sysret sys_linkat(
    int oldfdat, const char *oldpath, int newfdat, const char *newpath)
{
    struct dentry *olddentry = resolve_atpath(oldfdat, oldpath, true);
    if (IS_ERROR(olddentry))
        return ERROR(olddentry);

    if (!dentry_inode(olddentry))
        return -ENOENT;

    if (dentry_inode(olddentry)->type == FT_DIRECTORY)
        return -EISDIR;

    struct dentry *newdentry = resolve_atpath(newfdat, newpath, true);
    if (IS_ERROR(newdentry))
        return ERROR(newdentry);

    if (dentry_inode(newdentry))
        return -EEXIST;

    if (newdentry->file_system != olddentry->file_system)
        return -EXDEV;

    attach_inode(newdentry, olddentry->inode);
    return 0;
}

sysret sys_symlinkat(const char *topath, int newfdat, const char *newpath)
{
    struct dentry *dentry = resolve_atpath(newfdat, newpath, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);

    if (dentry_inode(dentry))
        return -EEXIST;

    struct inode *inode = new_inode(dentry->file_system, _NG_SYMLINK | 0777);
    inode->symlink_destination = strdup(topath);
    attach_inode(dentry, inode);
    return 0;
}

sysret sys_readlinkat(int atfd, const char *path, char *buffer, size_t len)
{
    struct dentry *dentry = resolve_atpath(atfd, path, false);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    struct inode *inode = dentry_inode(dentry);
    if (!inode)
        return -ENOENT;
    if (inode->type != FT_SYMLINK)
        return -EINVAL;

    readlink_inode(inode, buffer, len);

    // These can be dynamically generated, check if we don't need it
    // anymore
    maybe_delete_dentry(dentry);
    // return strnlen(buffer, len);
    return strlen(buffer);
}

sysret sys_mknodat(int atfd, const char *path, mode_t mode, dev_t device)
{
    struct dentry *dentry = resolve_atpath(atfd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    if (dentry_inode(dentry))
        return -EEXIST;

    struct file_system *file_system = dentry_file_system(dentry);
    struct inode *inode = new_inode(file_system, mode);
    int device_major = device >> 16;
    int device_minor = device & 0xFFFF;
    struct file_operations *drv_ops = char_drivers[device_major];
    if (!drv_ops)
        return -ENODEV;

    inode->device_major = device_major;
    inode->device_minor = device_minor;
    inode->file_ops = drv_ops;
    inode->type = FT_CHAR_DEV;

    attach_inode(dentry, inode);
    return 0;
}

sysret sys_pipe(int pipefds[static 2])
{
    struct inode *pipe = new_pipe();
    pipe->is_anon_pipe = true;
    struct file *read_end = no_d_file(pipe, O_RDONLY);
    struct file *write_end = no_d_file(pipe, O_WRONLY);
    pipefds[0] = add_file(read_end);
    pipefds[1] = add_file(write_end);
    return 0;
}

sysret sys_mkpipeat(int atfd, const char *path, mode_t mode)
{
    struct dentry *dentry = resolve_atpath(atfd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    if (dentry_inode(dentry))
        return -EEXIST;

    struct inode *pipe = new_pipe();
    pipe->is_anon_pipe = false;
    pipe->mode = mode;

    attach_inode(dentry, pipe);
    return 0;
}

sysret sys_mountat(
    int atfd, const char *target, int type, int s_atfd, const char *source)
{
    struct dentry *tdentry = resolve_atpath(atfd, target, true);
    if (IS_ERROR(tdentry))
        return ERROR(tdentry);
    struct inode *tinode = dentry_inode(tdentry);
    if (!tinode)
        return -ENOENT;
    if (tinode->type != FT_DIRECTORY)
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

sysret sys_dup(int fd)
{
    struct file *old = get_file(fd);
    if (!old)
        return -EBADF;
    struct file *new = clone_file(old);
    return add_file(new);
}

sysret sys_dup2(int fd, int newfd)
{
    struct file *old = get_file(fd);
    if (!old)
        return -EBADF;
    struct file *new = clone_file(old);

    struct file *close = get_file(newfd);
    if (close)
        close_file(close);
    return add_file_at(new, newfd);
}

sysret sys_fchmod(int fd, int mode)
{
    struct file *file = get_file(fd);
    if (!file)
        return -EBADF;
    if (!write_mode(file))
        return -EPERM;
    struct inode *inode = file->inode;
    inode->mode = (inode->mode & ~0xFFFF) | (mode & 0xFFFF);
    return 0;
}

sysret sys_chmodat(int atfd, const char *path, int mode)
{
    struct dentry *dentry = resolve_atpath(atfd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    struct inode *inode = dentry_inode(dentry);
    if (!inode)
        return -ENOENT;

    inode->mode = (inode->mode & ~0xFFFF) | (mode & 0xFFFF);
    return 0;
}

sysret sys_unlinkat(int atfd, const char *path, int mode)
{
    struct dentry *dentry = resolve_atpath(atfd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    struct inode *inode = dentry_inode(dentry);
    if (!inode)
        return -ENOENT;
    if (!write_permission(inode))
        return -EPERM;
    if (inode->type == FT_DIRECTORY)
        return -EISDIR;

    unlink_dentry(dentry);
    detach_inode(dentry);
    free(dentry);
    return 0;
}

// Given a POSITIVE dentry (extant inode), create a `struct file` to
// open it and return the new "file" object.
struct file *new_file(struct dentry *dentry, int flags)
{
    int err;
    struct file *file = malloc(sizeof(struct file));
    struct inode *inode = dentry_inode(dentry);
    *file = (struct file) {
        .inode = inode,
        .dentry = dentry,
        .flags = flags, // validate?
        .ops = inode->file_ops,
    };

    err = open_file(file);

    if (IS_ERROR(err)) {
        free(file);
        return TO_ERROR(err);
    }

    return file;
}

// Given a NEGATIVE dentry and an inode, associate the inode with the
// dentry, then open the file and return it.
struct file *create_file(struct dentry *dentry, struct inode *inode, int flags)
{
    attach_inode(dentry, inode);
    return new_file(dentry, flags);
}

// Create a file for an inode that has NO dentry (i.e. pipe)
struct file *no_d_file(struct inode *inode, int flags)
{
    int err;
    struct file *file = malloc(sizeof(struct file));
    *file = (struct file) {
        .inode = inode,
        .flags = flags, // validate?
        .ops = inode->file_ops,
    };

    err = open_file(file);

    if (IS_ERROR(err)) {
        free(file);
        return TO_ERROR(err);
    }

    return file;
}

// truncate file
void truncate(struct file *file) { file->inode->len = 0; }

// set to append, move cursor
void append(struct file *file) { file->offset = file->inode->len; }
