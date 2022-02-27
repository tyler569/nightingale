#include <stdlib.h>
#include "inode.h"

struct inode_operations default_ops = {0};

// eventually file_system->new_inode
struct inode *new_inode(int mode) {
    struct inode *inode = calloc(1, sizeof(struct inode));

    *inode = (struct inode) {
        .filesystem_id = 0,
        .mode = mode,
        .ops = &default_ops,
    };

    wq_init(&inode->read_queue);
    wq_init(&inode->write_queue);

    return inode;
}
