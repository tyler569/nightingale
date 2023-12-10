#include <errno.h>
#include <fcntl.h>
#include <ng/common.h>
#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/inode.h>
#include <ng/fs/proc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern file_system *proc_file_system;
extern file_operations proc_file_ops;
extern inode_operations proc_inode_ops;

inode *new_proc_inode(int mode, void (*generate)(file *, void *arg), void *arg)
{
    inode *inode = new_inode(proc_file_system, _NG_PROC | mode);
    inode->ops = &proc_inode_ops;
    inode->file_ops = &proc_file_ops;
    inode->extra = (void *)generate;
    inode->data = arg;
    return inode;
}

void make_proc_file(
    const char *name, void (*generate)(file *, void *arg), void *arg)
{
    dentry *root = proc_file_system->root;
    inode *inode = new_proc_inode(0444, generate, arg);
    dentry *dentry = resolve_path_from(root, name, true);
    if (dentry->inode) {
        printf("proc file '%s' already exists\n", name);
        maybe_delete_inode(inode);
        return;
    }
    attach_inode(dentry, inode);
}

ssize_t proc_file_read(file *file, char *buffer, size_t len)
{
    size_t to_read = MIN(file->len - file->offset, len);
    memcpy(buffer, PTR_ADD(file->extra, file->offset), to_read);
    file->offset += to_read;
    return to_read;
}

ssize_t proc_file_write(file *file, const char *buffer, size_t len)
{
    return -ETODO;
}

off_t proc_file_seek(file *file, off_t offset, int whence)
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

int proc_file_open(inode *inode, file *f)
{
    void (*generate)(file *, void *arg);
    f->extra = malloc(4096 * 4);
    f->size = 4096 * 4;
    f->len = 0;
    generate = (void (*)(file *, void *))inode->extra;
    generate(f, inode->data);
    return 0;
}

int proc_file_close(inode *inode, file *file)
{
    if (file->extra)
        free(file->extra);
    return 0;
}

file_operations proc_file_ops = {
    .read = proc_file_read,
    .write = proc_file_write,
    .seek = proc_file_seek,
};

inode_operations proc_inode_ops = {
    .open = proc_file_open,
    .close = proc_file_close,
};

void proc_sprintf(file *file, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    file->len += vsnprintf((char *)PTR_ADD(file->extra, file->len),
        file->size - file->len, fmt, args);
    // va_end is called in vsnprintf
}
