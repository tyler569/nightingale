#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <ng/common.h>
#include <ng/fs/char_dev.h>
#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/file_system.h>
#include <ng/fs/inode.h>
#include <ng/fs/pipe.h>
#include <ng/syscalls.h>
#include <ng/thread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// associate inode with NEGATIVE dentry
file *create_file(dentry *dentry, inode *inode, int flags);
// open existing inode
file *new_file(dentry *dentry, int flags);
// Create a file for an inode that has NO dentry (i.e. pipe)
file *no_d_file(inode *inode, int flags);
// truncate file
void truncate(file *file);
// set to append, move cursor
void append(file *file);

sysret do_open(dentry *cwd, const char *path, int flags, int mode)
{
    dentry *dentry = resolve_path_from(cwd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    inode *inode = dentry_inode(dentry);
    if (!inode && !(flags & O_CREAT))
        return -ENOENT;
    if (dentry && inode && flags & O_CREAT && flags & O_EXCL)
        return -EEXIST;

    file_system *file_system = dentry_file_system(dentry);
    file *file;

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
    dentry *root = resolve_atfd(fd);

    if (IS_ERROR(root))
        return ERROR(root);

    return do_open(root, path, flags, mode);
}

sysret do_touch(dentry *cwd, const char *path, int flags, int mode)
{
    dentry *dentry = resolve_path_from(cwd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    inode *inode = dentry_inode(dentry);

    assert(flags & O_CREAT && flags & O_EXCL);

    if (dentry && inode)
        return -EEXIST;

    file_system *file_system = dentry_file_system(dentry);

    if (!has_permission(dentry_inode(dentry->parent), flags | O_WRONLY))
        return -EPERM;

    inode = new_inode(file_system, mode);
    attach_inode(dentry, inode);
    return 0;
}

sysret sys_mkdirat(int fd, const char *path, int mode)
{
    dentry *root = resolve_atfd(fd);

    if (IS_ERROR(root))
        return ERROR(root);

    return do_touch(root, path, O_CREAT | O_EXCL, _NG_DIR | mode);
}

sysret sys_close(int fd)
{
    file *file = remove_file(fd);
    if (!file)
        return -EBADF;
    close_file(file);
    return 0;
}

sysret sys_getdents(int fd, dirent *buf, size_t len)
{
    file *directory = get_file(fd);
    if (!directory)
        return -EBADF;
    inode *inode = directory->inode;
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
    file *file = get_file(fd);
    if (!file)
        return -EBADF;
    dentry *dentry = file->dentry;
    if (!dentry)
        return -EINVAL;
    return pathname(dentry, buffer, len);
}

sysret sys_getcwd(char *buffer, size_t len)
{
    dentry *dentry = get_running_cwd();
    if (!dentry)
        return -EINVAL;
    return pathname(dentry, buffer, len);
}

sysret sys_chdirat(int atfd, const char *path)
{
    dentry *dentry = resolve_atpath(atfd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    if (!dentry_inode(dentry))
        return -ENOENT;

    // FIXME: reference count this!
    set_running_cwd(dentry);
    return 0;
}

sysret sys_read(int fd, void *buffer, size_t len)
{
    file *file = get_file(fd);
    if (!file)
        return -EBADF;

    return read_file(file, (char *)buffer, len);
}

sysret sys_write(int fd, const void *buffer, size_t len)
{
    file *file = get_file(fd);
    if (!file)
        return -EBADF;

    return write_file(file, (const char *)buffer, len);
}

sysret sys_ioctl(int fd, int request, void *argp)
{
    file *file = get_file(fd);
    if (!file)
        return -EBADF;

    return ioctl_file(file, request, argp);
}

sysret sys_lseek(int fd, off_t offset, int whence)
{
    file *file = get_file(fd);
    if (!file)
        return -EBADF;

    return seek_file(file, offset, whence);
}

sysret stat_inode(inode *inode, struct stat *stat)
{
    *stat = (struct stat) {
        .st_dev = (dev_t)(intptr_t)inode->file_system,
        .st_ino = inode->inode_number,
        .st_mode = (inode->mode & 0xFFFF) + (inode->type << 16),
        .st_nlink = inode->dentry_refcnt,
        .st_uid = inode->uid,
        .st_gid = inode->gid,
        .st_rdev
        = static_cast<dev_t>((inode->device_major << 16) + inode->device_minor),
        .st_size = static_cast<off_t>(inode->len),
        .st_blksize = 1024, // FIXME
        .st_blocks = ROUND_UP(inode->len, 1024) / 1024, // FIXME
        .st_atime = inode->atime,
        .st_mtime = inode->mtime,
        .st_ctime = inode->ctime,
    };
    return 0;
}

sysret sys_fstat(int fd, struct stat *stat)
{
    file *file = get_file(fd);
    if (!file)
        return -EBADF;

    inode *inode = file->inode;
    return stat_inode(inode, stat);
}

sysret sys_statat(int atfd, const char *path, struct stat *stat)
{
    dentry *dentry = resolve_atpath(atfd, path, false);
    if (IS_ERROR(dentry))
        return ERROR(dentry);

    inode *inode = dentry_inode(dentry);
    if (!inode)
        return -ENOENT;
    return stat_inode(inode, stat);
}

sysret sys_linkat(
    int oldfdat, const char *oldpath, int newfdat, const char *newpath)
{
    dentry *olddentry = resolve_atpath(oldfdat, oldpath, true);
    if (IS_ERROR(olddentry))
        return ERROR(olddentry);

    if (!dentry_inode(olddentry))
        return -ENOENT;

    if (dentry_inode(olddentry)->type == FT_DIRECTORY)
        return -EISDIR;

    dentry *newdentry = resolve_atpath(newfdat, newpath, true);
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
    dentry *dentry = resolve_atpath(newfdat, newpath, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);

    if (dentry_inode(dentry))
        return -EEXIST;

    inode *inode = new_inode(dentry->file_system, _NG_SYMLINK | 0777);
    inode->symlink_destination = strdup(topath);
    attach_inode(dentry, inode);
    return 0;
}

sysret sys_readlinkat(int atfd, const char *path, char *buffer, size_t len)
{
    dentry *dentry = resolve_atpath(atfd, path, false);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    inode *inode = dentry_inode(dentry);
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
    dentry *dentry = resolve_atpath(atfd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    if (dentry_inode(dentry))
        return -EEXIST;

    file_system *file_system = dentry_file_system(dentry);
    inode *inode = new_inode(file_system, mode);
    int device_major = device >> 16;
    int device_minor = device & 0xFFFF;
    file_operations *drv_ops = char_drivers[device_major];
    if (!drv_ops)
        return -ENODEV;

    inode->device_major = device_major;
    inode->device_minor = device_minor;
    inode->file_ops = drv_ops;
    inode->type = FT_CHAR_DEV;

    attach_inode(dentry, inode);
    return 0;
}

sysret sys_pipe(int pipefds[])
{
    inode *pipe = new_pipe();
    pipe->is_anon_pipe = true;
    file *read_end = no_d_file(pipe, O_RDONLY);
    file *write_end = no_d_file(pipe, O_WRONLY);
    pipefds[0] = add_file(read_end);
    pipefds[1] = add_file(write_end);
    return 0;
}

sysret sys_mkpipeat(int atfd, const char *path, mode_t mode)
{
    dentry *dentry = resolve_atpath(atfd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    if (dentry_inode(dentry))
        return -EEXIST;

    inode *pipe = new_pipe();
    pipe->is_anon_pipe = false;
    pipe->mode = mode;

    attach_inode(dentry, pipe);
    return 0;
}

sysret sys_mountat(
    int atfd, const char *target, int type, int s_atfd, const char *source)
{
    dentry *tdentry = resolve_atpath(atfd, target, true);
    if (IS_ERROR(tdentry))
        return ERROR(tdentry);
    inode *tinode = dentry_inode(tdentry);
    if (!tinode)
        return -ENOENT;
    if (tinode->type != FT_DIRECTORY)
        return -ENOTDIR;

    file_system *file_system;
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
    file *old = get_file(fd);
    if (!old)
        return -EBADF;
    file *new_f = clone_file(old);
    return add_file(new_f);
}

sysret sys_dup2(int fd, int newfd)
{
    file *old = get_file(fd);
    if (!old)
        return -EBADF;
    file *new_f = clone_file(old);

    file *close = get_file(newfd);
    if (close)
        close_file(close);
    return add_file_at(new_f, newfd);
}

sysret sys_fchmod(int fd, int mode)
{
    file *file = get_file(fd);
    if (!file)
        return -EBADF;
    if (!write_mode(file))
        return -EPERM;
    inode *inode = file->inode;
    inode->mode = (inode->mode & ~0xFFFF) | (mode & 0xFFFF);
    return 0;
}

sysret sys_chmodat(int atfd, const char *path, int mode)
{
    dentry *dentry = resolve_atpath(atfd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    inode *inode = dentry_inode(dentry);
    if (!inode)
        return -ENOENT;

    inode->mode = (inode->mode & ~0xFFFF) | (mode & 0xFFFF);
    return 0;
}

sysret sys_unlinkat(int atfd, const char *path)
{
    dentry *dentry = resolve_atpath(atfd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    inode *inode = dentry_inode(dentry);
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

// Given a POSITIVE dentry (extant inode), create a `file` to
// open it and return the new "file" object.
file *new_file(dentry *dentry, int flags)
{
    int err;
    auto *f = (file *)malloc(sizeof(file));
    inode *inode = dentry_inode(dentry);
    *f = (file) {
        .dentry = dentry,
        .inode = inode,
        .ops = inode->file_ops,
        .flags = static_cast<open_flags>(flags), // validate?
    };

    err = open_file(f);

    if (IS_ERROR(err)) {
        free(f);
        return (file *)(TO_ERROR(err));
    }

    return f;
}

// Given a NEGATIVE dentry and an inode, associate the inode with the
// dentry, then open the file and return it.
file *create_file(dentry *dentry, inode *inode, int flags)
{
    attach_inode(dentry, inode);
    return new_file(dentry, flags);
}

// Create a file for an inode that has NO dentry (i.e. pipe)
file *no_d_file(inode *inode, int flags)
{
    int err;
    file *f = (file *)malloc(sizeof(file));
    *f = (file) {
        .inode = inode,
        .ops = inode->file_ops,
        .flags = static_cast<open_flags>(flags),
    };

    err = open_file(f);

    if (IS_ERROR(err)) {
        free(f);
        return (file *)TO_ERROR(err);
    }

    return f;
}

// truncate file
void truncate(file *file) { file->inode->len = 0; }

// set to append, move cursor
void append(file *file) { file->offset = file->inode->len; }
