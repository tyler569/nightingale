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



ssize_t default_read(struct fs2_file *file, char *buffer, size_t len) {
    if (file->offset > file->inode->len)
        return 0;
    size_t to_read = umin(len, file->inode->len - file->offset);
    memcpy(buffer, PTR_ADD(file->inode->data, file->offset), to_read);
    file->offset += to_read;
    return to_read;
}

ssize_t default_write(struct fs2_file *file, const char *buffer, size_t len) {
    if (!file->inode->data) {
        file->inode->data = malloc(1024);
        file->inode->capacity = 1024;
    }

    size_t final_len = file->offset + len;
    if (file->offset + len > file->inode->capacity) {
        size_t resized_len = final_len * 3 / 2;
        file->inode->data = realloc(file->inode->data, resized_len);
        file->inode->capacity = resized_len;
    }

    memcpy(PTR_ADD(file->inode->data, file->offset), buffer, len);
    file->offset += len;
    file->inode->len = umax(file->inode->len, final_len);
    return len;
}


struct file_operations default_file_ops = {0};

bool read_mode(struct fs2_file *fs2_file) {
    return fs2_file->flags & O_RDONLY;
}

bool write_mode(struct fs2_file *fs2_file) {
    return fs2_file->flags & O_WRONLY;
}

bool has_permission(struct inode *inode, int flags) {
    // bootleg implies, truth table:
    // mode  flags allowed
    // 0     0     1
    // 0     1     0
    // 1     0     1
    // 1     1     1

    return (inode->mode & USR_READ || !(flags & O_RDONLY)) &&
        (inode->mode & USR_WRITE || !(flags & O_WRONLY));
}

bool execute_permission(struct inode *inode) {
    return inode->mode & USR_EXEC;
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

    running_process->fs2_files = zrealloc(fds, new_max * sizeof(void *));
    running_process->fs2_files[prev_max] = fs2_file;
    running_process->n_fd2s = new_max;
    return prev_max;
}

struct fs2_file *remove_file(int fd) {
    struct fs2_file **fds = running_process->fs2_files;

    struct fs2_file *file = fds[fd];

    fds[fd] = 0;
    return file;
}

ssize_t read_file(struct fs2_file *file, char *buffer, size_t len) {
    if (!read_mode(file))
        return -EPERM;

    if (file->ops->read)
        return file->ops->read(file, buffer, len);
    else
        return default_read(file, buffer, len);
}

ssize_t write_file(struct fs2_file *file, const char *buffer, size_t len) {
    if (!write_mode(file))
        return -EPERM;

    if (file->ops->write)
        return file->ops->write(file, buffer, len);
    else
        return default_write(file, buffer, len);
}

int ioctl_file(struct fs2_file *file, int request, void *argp) {
    if (file->ops->ioctl)
        return file->ops->ioctl(file, request, argp);
    else
        return -ENOTTY;
}
