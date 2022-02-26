#include <basic.h>
#include <dirent.h>
#include <list.h>
#include <ng/string.h>
#include <ng/thread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dentry.h"
#include "inode.h"
#include "file.h"

#define file fs2_file

struct dentry *resolve_path_from(struct dentry *cursor, const char *path);
struct dentry *resolve_path(const char *path);


// truncate file
void truncate(struct file *file);
// set to append, move cursor
void append(struct file *file);


#define DIR 1

sysret do_open2(struct file *cwd, const char *path, int flags, int mode) {
    struct dentry *dentry = resolve_path_from(cwd->dentry, path);

    if (!dentry || !dentry->inode && flags & O_CREAT) {
        return -ENOENT;
    }
    // TODO permissions checking

    struct file *file;

    if (O_CREAT) {
        file = create_file(dentry, new_inode(mode), flags);
    } else {
        file = new_file(dentry, flags);
    }

    if (O_TRUNC)
        truncate(file);

    if (O_APPEND)
        append(file);

    return add_file(file);
}


sysret sys_open2(const char *path, int flags, int mode) {
    return do_open2(running_thread->cwd2, path, flags, mode);
}

sysret sys_openat2(int fd, const char *path, int flags, int mode) {
    struct file *file = get_file(fd);
    if (!file)
        return -EBADF;

    return do_open2(file, path, flags, mode);
}






sysret sys_getdents2(int fd, struct ng_dirent *dents, size_t len) {
    struct file *directory = get_file(fd);
    if (!file)
        return -EBADF;

    if (!(directory->inode->flags & DIR)) {
        return 0;
    }

    // TODO permissions checking on directory

    size_t index = 0;
    list_for_each(struct dentry, d, &directory->inode->children, children_node) {
        if (!d->inode) {
            continue;
        }
        strncpy(dents[index].name, d->name, 128);
        dents[index].type = d->inode->flags;
        index += 1;

        if (index == len) {
            break;
        }
    }
    return index;
}





sysret sys_pathname2(int fd, char *buffer, size_t len) {
    struct file *file = get_file(fd);
    if (!file)
        return -EBADF;

    struct dentry *dentry = file->dentry;

    pathname(file, buffer, len);
}













struct file *get_file(int fd) {
    if (fd > running_process->n_fd2s) {
        return NULL;
    }

    return running_process->fs2_files[fd];
}

int add_file(struct file *file) {
    struct file *fds = running_process->fs2_files;
    
    for (int i = 0; i < running_process->n_fd2s; i++) {
        if (!fds[i]) {
            fds[i] = file;
            return i;
        }
    }

    int prev_max = running_process->n_fd2s;
    int new_max = prev_max * 2;

    running_process->fs2_files = realloc(fds, new_max);
    running_process->fs2_files[prev_max] = file;
    return prev_max;
}


struct file *new_file(struct dentry *dentry, int flags) {
    struct file *file = malloc(sizeof(struct file));
    file->inode = dentry->inode;
    file->dentry = dentry;
    file->flags = flags; // validate?
    file->ops = dentry->inode->file_ops;

    // inode->ops->open(inode, file);
}

struct file *create_file(struct dentry *dentry, struct inode *inode) {
    dentry->inode = inode;
    return new_file(dentry);
}

















struct dentry *resolve_path(const char *path) {
    return resolve_path_from(&global_root_dentry, path);
}







struct file *create_file(struct file *file, const char *name, int flags) {
    struct dentry *cursor = file->dentry;
    struct inode *inode = malloc(sizeof(struct inode));
    inode->flags = flags;
    if (flags & DIR) {
        list_init(&inode->children);
    }
    struct dentry *new_dentry = add_child(cursor, name, inode);
    if (!new_dentry) {
        return NULL;
    }
    return new_file(new_dentry);
}

















char *strccpy(char *dest, const char *src, int c) {
    while (*src && *src != c) {
        *dest++ = *src++;
    }
    *dest = 0;
    if (*src == c) {
        src++;
    }
    return (char *)src;
}

char *strcncpy(char *dest, const char *src, int c, size_t len) {
    size_t n = 0;
    while (*src && *src != c && n < len) {
        *dest++ = *src++;
        n++;
    }
    if (n < len) {
        *dest = 0;
        if (*src == c)
            src++;
    }
    return (char *)src;
}
