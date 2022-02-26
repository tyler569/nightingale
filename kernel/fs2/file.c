#include <basic.h>
#include <errno.h>
#include "types.h"
#include "file.h"
#include "inode.h"
#include "dentry.h"

#define file fs2_file

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






bool read_permission(struct file *file) {
    return true;
}

bool write_permission(struct file *file) {
    return true;
}

bool execute_permission(struct file *file) {
    return true;
}
