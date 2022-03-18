#include <stdlib.h>
#include <ng/fs/tmpfs.h>
#include <ng/fs/dentry.h>
#include <ng/fs/file_system.h>
#include <ng/fs/inode.h>

struct file_system *new_tmpfs_file_system(void)
{
    struct file_system *file_system = zmalloc(sizeof(struct file_system));
    file_system->ops = &default_file_system_ops;
    file_system->next_inode_number = 2;
    list_init(&file_system->inodes);

    struct inode *root_inode = new_inode(file_system, _NG_DIR | 0755);
    struct dentry *root = new_dentry();
    root->parent = root;
    root->file_system = file_system;
    attach_inode(root, root_inode);
    file_system->root = root;

    return file_system;
}
