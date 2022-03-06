#include <stdlib.h>
#include "dentry.h"
#include "inode.h"
#include "file.h"

struct inode_operations default_ops = {0};

// eventually file_system->new_inode
struct inode *new_inode(int flags, int mode) {
    struct inode *inode = calloc(1, sizeof(struct inode));

    *inode = (struct inode) {
        .filesystem_id = 0,
        .mode = mode,
        .ops = &default_ops,
        .file_ops = &default_file_ops,
    };

    if (flags & _NG_DIR) {
        inode->type = FT_DIRECTORY;
    } else {
        inode->type = FT_NORMAL;
    }

    wq_init(&inode->read_queue);
    wq_init(&inode->write_queue);

    return inode;
}

int open_file(struct fs2_file *file) {
    // TODO: should the dentry refcounts be managed by inode.c?
    atomic_fetch_add_explicit(&file->dentry->file_refcnt, 1, memory_order_acquire);
    if (file->inode->ops->open)
        file->inode->ops->open(file->inode, file);
    return 0;
}

int close_file(struct fs2_file *file) {
    // TODO: should the dentry refcounts be managed by inode.c?
    atomic_fetch_sub_explicit(&file->dentry->file_refcnt, 1, memory_order_release);
    if (file->inode->ops->close)
        file->inode->ops->close(file->inode, file);
    return 0;
}
