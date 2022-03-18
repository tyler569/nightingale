#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "proc.h"
#include "dentry.h"
#include "file.h"
#include "inode.h"

extern struct file_system *proc_file_system;
struct file_operations proc_file_ops;
struct inode_operations proc_inode_ops;

struct inode *new_proc_file(
    int mode, void (*generate)(struct fs2_file *, void *arg), void *arg)
{
    struct inode *inode = new_inode(proc_file_system, _NG_PROC | mode);
    inode->ops = &proc_inode_ops;
    inode->file_ops = &proc_file_ops;
    inode->extra = generate;
    inode->data = arg;
    return inode;
}

void make_proc_file2(
    const char *name, void (*generate)(struct fs2_file *, void *arg), void *arg)
{
    struct dentry *root = proc_file_system->root;
    struct inode *inode = new_proc_file(0444, generate, arg);
    struct dentry *dentry = resolve_path_from(root, name, true);
    if (dentry->inode) {
        printf("proc file '%s' already exists\n", name);
        free(inode);
        return;
    }
    attach_inode(dentry, inode);
}

ssize_t proc_file_read(struct fs2_file *file, char *buffer, size_t len)
{
    size_t to_read = min(file->len - file->offset, len);
    memcpy(buffer, PTR_ADD(file->extra, file->offset), to_read);
    file->offset += to_read;
    return to_read;
}

ssize_t proc_file_write(struct fs2_file *file, const char *buffer, size_t len)
{
    return -ETODO;
}

off_t proc_file_seek(struct fs2_file *file, off_t offset, int whence)
{
    off_t new_offset = file->offset;

    switch (whence) {
    case SEEK_SET:
        new_offset = offset;
        break;
    case SEEK_CUR:
        new_offset += offset;
        break;
    case SEEK_END:
        // The only difference between this and default_seek is that
        // this uses file->len for SEEK_END because the inode has no
        // len of its own.
        new_offset = file->len + offset;
        break;
    default:
        return -EINVAL;
    }

    if (new_offset < 0)
        return -EINVAL;

    file->offset = new_offset;
    return new_offset;
}

int proc_file_open(struct inode *inode, struct fs2_file *file)
{
    void (*generate)(struct fs2_file *, void *arg);
    file->extra = malloc(4096 * 4);
    file->size = 4096 * 4;
    file->len = 0;
    generate = inode->extra;
    generate(file, inode->data);
    return 0;
}

int proc_file_close(struct inode *inode, struct fs2_file *file)
{
    if (file->extra)
        free(file->extra);
    return 0;
}

struct file_operations proc_file_ops = {
    .read = proc_file_read,
    .write = proc_file_write,
    .seek = proc_file_seek,
};

struct inode_operations proc_inode_ops = {
    .open = proc_file_open,
    .close = proc_file_close,
};

void proc2_sprintf(struct fs2_file *file, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    file->len += vsnprintf(
        file->extra + file->len, file->size - file->len, fmt, args);
    // va_end is called in vsnprintf
}
