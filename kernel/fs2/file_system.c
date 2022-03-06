#include <list.h>
#include <stdlib.h>
#include "file_system.h"
#include "file.h"
#include "inode.h"
#include "dentry.h"

struct file_system_operations default_file_system_ops = {0};
struct file_system *initfs_file_system;

struct inode *new_inode(struct file_system *file_system, int flags, int mode) {
    struct inode *inode;
    if (file_system->ops->new_inode) {
        inode = file_system->ops->new_inode(file_system);
    } else {
        inode = calloc(1, sizeof(struct inode));
    }

    inode->file_system = file_system;
    inode->mode = mode;
    inode->ops = &default_ops;
    inode->file_ops = &default_file_ops;

    if (flags & _NG_DIR) {
        inode->type = FT_DIRECTORY;
    } else {
        inode->type = FT_NORMAL;
    }

    wq_init(&inode->read_queue);
    wq_init(&inode->write_queue);

    return inode;
}
