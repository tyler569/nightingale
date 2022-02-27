#include <basic.h>
#include <dirent.h>
#include <fcntl.h>
#include <list.h>
#include <ng/string.h>
#include <ng/thread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dentry.h"
#include "inode.h"
#include "file.h"

struct dentry *resolve_path_from(struct dentry *cursor, const char *path);
struct dentry *resolve_path(const char *path);


// associate inode with NEGATIVE dentry dentry
struct fs2_file *create_file2(
    struct dentry *dentry,
    struct inode *inode,
    int flags
);
// open existing inode
struct fs2_file *new_file(struct dentry *dentry, int flags);
// truncate fs2_file
void truncate(struct fs2_file *fs2_file);
// set to append, move cursor
void append(struct fs2_file *fs2_file);


#define DIR 1

sysret do_open2(struct fs2_file *cwd, const char *path, int flags, int mode) {
    struct dentry *dentry = resolve_path_from(cwd->dentry, path);

    if (!dentry || (!dentry->inode && flags & O_CREAT)) {
        return -ENOENT;
    }
    // TODO permissions checking

    struct fs2_file *fs2_file;

    if (O_CREAT) {
        fs2_file = create_file2(dentry, new_inode(mode), flags);
    } else {
        fs2_file = new_file(dentry, flags);
    }

    if (O_TRUNC)
        truncate(fs2_file);

    if (O_APPEND)
        append(fs2_file);

    return add_file(fs2_file);
}


sysret sys_open2(const char *path, int flags, int mode) {
    return do_open2(running_thread->cwd2, path, flags, mode);
}

sysret sys_openat2(int fd, const char *path, int flags, int mode) {
    struct fs2_file *fs2_file = get_file(fd);
    if (!fs2_file)
        return -EBADF;

    return do_open2(fs2_file, path, flags, mode);
}






sysret sys_getdents2(int fd, struct ng_dirent *dents, size_t len) {
    struct fs2_file *directory = get_file(fd);
    if (!directory)
        return -EBADF;

    if (!(directory->inode->flags & DIR)) {
        return 0;
    }

    // TODO permissions checking on directory

    size_t index = 0;
    list_for_each(
        struct dentry,
        d,
        &directory->inode->children,
        children_node
    ) {
        if (!d->inode) {
            continue;
        }
        strncpy(dents[index].name, d->name, 128);
        dents[index].type = d->inode->type;
        index += 1;

        if (index == len) {
            break;
        }
    }
    return index;
}





sysret sys_pathname2(int fd, char *buffer, size_t len) {
    struct fs2_file *fs2_file = get_file(fd);
    if (!fs2_file)
        return -EBADF;

    struct dentry *dentry = fs2_file->dentry;

    return pathname(fs2_file, buffer, len);
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


struct fs2_file *new_file(struct dentry *dentry, int flags) {
    struct fs2_file *fs2_file = malloc(sizeof(struct fs2_file));
    *fs2_file = (struct fs2_file) {
        .inode = dentry->inode,
        .dentry = dentry,
        .flags = flags, // validate?
        .ops = dentry->inode->file_ops,
    };

    // inode->ops->open(inode, fs2_file);
    return fs2_file;
}

struct fs2_file *create_file2(
    struct dentry *dentry,
    struct inode *inode,
    int flags
) {
    dentry->inode = inode;
    return new_file(dentry, 0);
}

// struct fs2_file *create_file2(struct fs2_file *fs2_file, const char *name, int flags) {
//     struct dentry *cursor = fs2_file->dentry;
//     struct inode *inode = malloc(sizeof(struct inode));
//     inode->flags = flags;
//     if (flags & DIR) {
//         list_init(&inode->children);
//     }
//     struct dentry *new_dentry = add_child(cursor, name, inode);
//     if (!new_dentry) {
//         return NULL;
//     }
//     return new_file(new_dentry);
// }




struct dentry *resolve_path(const char *path) {
    return resolve_path_from(running_process->root, path);
}







// truncate fs2_file
void truncate(struct fs2_file *fs2_file) {
    fs2_file->inode->len = 0;
}

// set to append, move cursor
void append(struct fs2_file *fs2_file) {
    fs2_file->offset = fs2_file->inode->len;
}
