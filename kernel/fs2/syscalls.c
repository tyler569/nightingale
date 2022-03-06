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


sysret do_open2(struct dentry *cwd, const char *path, int flags, int mode) {
    struct dentry *dentry = resolve_path_from(cwd, path);

    if (!dentry || (!dentry->inode && !(flags & O_CREAT))) {
        return -ENOENT;
    }
    if (dentry && dentry->inode && flags & O_CREAT && flags & O_EXCL) {
        return -EEXIST;
    }

    // TODO permissions

    struct fs2_file *fs2_file;

    if (flags & O_CREAT) {
        fs2_file = create_file2(dentry, new_inode(flags, mode), flags);
    } else {
        fs2_file = new_file(dentry, flags);
    }

    if (flags & O_TRUNC)
        truncate(fs2_file);

    if (flags & O_APPEND)
        append(fs2_file);

    open_file(fs2_file);

    return add_file(fs2_file);
}

sysret sys_openat2(int fd, const char *path, int flags, int mode) {
    struct dentry *root = resolve_atfd(fd);

    if (IS_ERROR(root))
        return ERROR(root);

    return do_open2(root, path, flags, mode);
}

sysret sys_mkdirat2(int fd, const char *path, int mode) {
    struct dentry *root = resolve_atfd(fd);

    if (IS_ERROR(root))
        return ERROR(root);

    return do_open2(root, path, O_CREAT | O_EXCL | _NG_DIR, mode);
}

sysret sys_close2(int fd) {
    struct fs2_file *file = remove_file(fd);
    close_file(file);
    return 0;
}



sysret sys_getdents2(int fd, struct ng_dirent *dents, size_t len) {
    struct fs2_file *directory = get_file(fd);
    if (!directory)
        return -EBADF;

    if (directory->inode->type != FT_DIRECTORY) {
        return 0;
    }

    // TODO permissions checking on directory

    size_t index = 0;
    list_for_each(
        struct dentry,
        d,
        &directory->dentry->children,
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

sysret fs2_read(struct fs2_file *file, char *buffer, size_t len) {
    if (!read_permission(file))
        return -EPERM;
    
    if (file->ops->read)
        return file->ops->read(file, buffer, len);
    else
        return default_read(file, buffer, len);
}

sysret fs2_write(struct fs2_file *file, char *buffer, size_t len) {
    if (!write_permission(file))
        return -EPERM;

    if (file->ops->write)
        return file->ops->write(file, buffer, len);
    else
        return default_write(file, buffer, len);
}

sysret sys_read2(int fd, char *buffer, size_t len) {
    struct fs2_file *file = get_file(fd);
    if (!file)
        return -EBADF;

    return fs2_read(file, buffer, len);
}

sysret sys_write2(int fd, char *buffer, size_t len) {
    struct fs2_file *file = get_file(fd);
    if (!file)
        return -EBADF;
    
    return fs2_write(file, buffer, len);
}












// Given a POSITIVE dentry (extant inode), create a `struct file` to
// open it and return the new "file" object.
struct fs2_file *new_file(struct dentry *dentry, int flags) {
    struct fs2_file *fs2_file = malloc(sizeof(struct fs2_file));
    struct inode *inode = dentry_inode(dentry);
    *fs2_file = (struct fs2_file) {
        .inode = inode,
        .dentry = dentry,
        .flags = flags, // validate?
        .ops = dentry->inode->file_ops,
    };

    open_file(fs2_file);
    return fs2_file;
}

// Given a NEGATIVE dentry and an inode, associate the inode with the
// dentry, then open the file and return it.
struct fs2_file *create_file2(
    struct dentry *dentry,
    struct inode *inode,
    int flags
) {
    dentry->inode = inode;
    return new_file(dentry, 0);
}



// truncate fs2_file
void truncate(struct fs2_file *fs2_file) {
    fs2_file->inode->len = 0;
}

// set to append, move cursor
void append(struct fs2_file *fs2_file) {
    fs2_file->offset = fs2_file->inode->len;
}
