#include <stdlib.h>
#include "file_system.h"
#include "tmpfs.h"

struct file_system *new_tmpfs_file_system(void) {
    struct file_system *file_system = zmalloc(sizeof(struct file_system));
    file_system->ops = &default_file_system_ops;
    file_system->next_inode_number = 3;
    return file_system;
}
