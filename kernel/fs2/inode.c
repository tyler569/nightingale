#include <stdlib.h>
#include "inode.h"

struct inode_operations default_ops = {
};

// eventually file_system->new_inode
struct inode *new_inode(int mode) {
    struct inode *inode = calloc(1, sizeof(struct inode));

    inode->filesystem_id = 0;
    inode->mode = mode;
    inode->ops = &default_ops;

    wq_init(&inode->read_queue);
    wq_init(&inode->write_queue);
}

struct inode {
    int filesystem_id;
    int mode;
    int uid;
    int gid;
    const struct inode_operations *ops;
    const struct file_operations *file_ops;
    waitqueue_t read_queue;
    waitqueue_t write_queue;

    void *extra;
};
