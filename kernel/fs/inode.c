#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/inode.h>
#include <stdlib.h>

struct inode_operations default_ops = { 0 };

static int close_file_refcounts(struct file *file);

static int open_file_refcounts(struct file *file)
{
    // TODO: should the dentry refcounts be managed by inode.c?
    if (file->dentry)
        atomic_fetch_add(&file->dentry->file_refcnt, 1);

    if (file->flags & O_RDONLY)
        atomic_fetch_add(&file->inode->read_refcnt, 1);
    if (file->flags & O_WRONLY)
        atomic_fetch_add(&file->inode->write_refcnt, 1);
    return 0;
}

int open_file_clone(struct file *file) { return open_file_refcounts(file); }

int open_file(struct file *file)
{
    int err = 0;
    open_file_refcounts(file);

    if (file->inode->ops->open)
        err = file->inode->ops->open(file->inode, file);

    // What should the "open fails" pattern be?
    // if (IS_ERROR(err))
    //     close_file_refcounts(file);

    return err;
}

static int close_file_refcounts(struct file *file)
{
    // TODO: should the dentry refcounts be managed by inode.c?
    if (file->dentry)
        atomic_fetch_sub(&file->dentry->file_refcnt, 1);

    if (file->flags & O_RDONLY)
        atomic_fetch_sub(&file->inode->read_refcnt, 1);
    if (file->flags & O_WRONLY)
        atomic_fetch_sub(&file->inode->write_refcnt, 1);

    return 0;
}

int close_file(struct file *file)
{
    int err = 0;
    close_file_refcounts(file);

    if (file->inode->ops->close)
        err = file->inode->ops->close(file->inode, file);

    if (file->dentry)
        maybe_delete_dentry(file->dentry);
    else
        maybe_delete_inode(file->inode);

    return err;
}

void maybe_delete_inode(struct inode *inode)
{
    if (inode->dentry_refcnt)
        return;
    if (inode->read_refcnt)
        return;
    if (inode->write_refcnt)
        return;

    // inode->delete() that writes back to disk etc.?

    //#include <stdio.h>
    // printf("inode (%c) freed\n", __filetype_sigils[inode->type]);
    free(inode);
}
