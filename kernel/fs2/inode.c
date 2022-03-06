#include <stdlib.h>
#include "dentry.h"
#include "inode.h"
#include "file.h"

struct inode_operations default_ops = {0};

int open_file(struct fs2_file *file) {
    // TODO: should the dentry refcounts be managed by inode.c?
    atomic_fetch_add_explicit(
        &file->dentry->file_refcnt,
        1,
        memory_order_acquire
    );
    if (file->inode->ops->open)
        file->inode->ops->open(file->inode, file);
    return 0;
}

int close_file(struct fs2_file *file) {
    // TODO: should the dentry refcounts be managed by inode.c?
    atomic_fetch_sub_explicit(
        &file->dentry->file_refcnt,
        1,
        memory_order_release
    );
    if (file->inode->ops->close)
        file->inode->ops->close(file->inode, file);
    return 0;
}
