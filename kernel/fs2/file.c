#include <basic.h>
#include "types.h"
#include "file.h"
#include "dentry.h"

// TO DO
// bool read_permission(file);
// bool write_permission(file);
// (etc)
//
// (interface) resolve_path(root, name)


void make_empty_file(const char *path) {
    struct dentry *cursor = vfs_root;
    struct inode *directory;
    int mode = 0755;

    resolve_path(&cursor, &directory, path);

    directory->create(directory, cursor, mode);
}







// Get a file from the running_process's fd table
struct file *get_file(int fd);
// Get a file from someone else's fd table (?)
struct file *get_file_from_process(int fd, struct process *process);

ssize_t sys_read(int fd, char *buffer, size_t len) {
    struct file *file = get_file(fd);
    if (!file)  return -EBADF;
    if (!read_permission(file))  return -EPERM;

    return file->ops->read(file, buffer, len);
}

ssize_t sys_write(int fd, const char *buffer, size_t len) {
    struct file *file = get_file(fd);
    if (!file)  return -EBADF;
    if (!write_permission(file))  return -EPERM;

    return file->ops->write(file, buffer, len);
}

int sys_open(const char *name, int flags, int mode) {
    struct dentry *cursor;
    struct inode *directory;
    if (name[0] == '/') {
        cursor = vfs_root;
    } else {
        cursor = running_process->cwd;
    }

    int status = resolve_path(&cursor, &directory, name);
    if (status < 0)  return status;

    if (!cursor->inode && flags & O_CREAT) {
        return directory->ops->create(directory, cursor, mode);
    }

    if (flags & O_CREAT && flags & O_EXCL) {
        return -EEXIST;
    }

    struct inode *inode = cursor->inode;
    struct file *file = inode->ops->open(inode, flags);
    if (is_error(file))  return ptr_error(file);
    return add_file(file);
}

struct inode *dentry_inode(struct dentry *dentry) {
    if (dentry->flags & DENTRY_IS_MOUNTPOINT) {
        return dentry->file_system->root_inode;
    } else {
        return dentry->inode;
    }
}

struct dentry vfs_root = {
    .name = "",
    .parent = &vfs_root;
};

void mount_filesystem(struct file_system *fs, struct dentry *mountpoint) {
    // mountpoint->mounted_filesystem = fs;
    // mountpoint->flags |= DENTRY_IS_MOUNTPOINT;
    // fs->ops->mount(fs, mountpoint);
}

void init_vfs() {
    struct dentry *slash_dev;

    mount_filesystem(initfs, vfs_root);
    slash_dev = lookup_child(vfs_root, "dev");
    mount_filesystem(devfs, slash_dev);
}


///// usermode 


int main() {
    mount("/proc", "proc");
    mount("/sys", "sys");
}
