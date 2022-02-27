#include <basic.h>
#include <errno.h>
#include <ng/thread.h>
#include "types.h"
#include "file.h"
#include "inode.h"
#include "dentry.h"

// Get a fs2_file from the running_process's fd table
struct fs2_file *get_file(int fd);
// Get a fs2_file from someone else's fd table (?)
struct fs2_file *get_file_from_process(int fd, struct process *process);

// ssize_t sys_read(int fd, char *buffer, size_t len) {
//     struct fs2_file *fs2_file = get_file(fd);
//     if (!fs2_file)  return -EBADF;
//     if (!read_permission(fs2_file))  return -EPERM;
//
//     return fs2_file->ops->read(fs2_file, buffer, len);
// }
//
// ssize_t sys_write(int fd, const char *buffer, size_t len) {
//     struct fs2_file *fs2_file = get_file(fd);
//     if (!fs2_file)  return -EBADF;
//     if (!write_permission(fs2_file))  return -EPERM;
//
//     return fs2_file->ops->write(fs2_file, buffer, len);
// }


bool read_permission(struct fs2_file *fs2_file) {
    return true;
}

bool write_permission(struct fs2_file *fs2_file) {
    return true;
}

bool execute_permission(struct fs2_file *fs2_file) {
    return true;
}
