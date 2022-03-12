#include <stdlib.h>
#include "file_system.h"
#include "inode.h"
#include "tmpfs.h"

struct file_system *new_tmpfs_file_system(void) {
    struct file_system *file_system = zmalloc(sizeof(struct file_system));
    file_system->ops = &default_file_system_ops;
    file_system->next_inode_number = 2;
    list_init(&file_system->inodes);

    struct inode *root_inode = new_inode(file_system, _NG_DIR | 0755);
    file_system->root_inode = root_inode;

    return file_system;
}
