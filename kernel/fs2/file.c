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

struct fs2_file *get_file(int fd) {
    if (fd > running_process->n_fd2s) {
        return NULL;
    }

    return running_process->fs2_files[fd];
}

int add_file(struct fs2_file *fs2_file) {
    struct fs2_file **fds = running_process->fs2_files;

    for (int i = 0; i < running_process->n_fd2s; i++) {
        if (!fds[i]) {
            fds[i] = fs2_file;
            return i;
        }
    }

    int prev_max = running_process->n_fd2s;
    int new_max = prev_max * 2;

    running_process->fs2_files = realloc(fds, new_max);
    running_process->fs2_files[prev_max] = fs2_file;
    return prev_max;
}

struct fs2_file *remove_file(int fd) {
    struct fs2_file **fds = running_process->fs2_files;

    struct fs2_file *file = fds[fd];

    fds[fd] = 0;
    return file;
}
