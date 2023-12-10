#include <ng/fs/dentry.h>
#include <ng/fs/file_system.h>
#include <ng/fs/inode.h>
#include <ng/fs/tmpfs.h>
#include <stdlib.h>

file_system *new_tmpfs_file_system(void)
{
    auto *fs = (file_system *)zmalloc(sizeof(file_system));
    fs->ops = &default_file_system_ops;
    fs->next_inode_number = 2;
    list_init(&fs->inodes);

    inode *root_inode = new_inode(fs, _NG_DIR | 0755);
    dentry *root = new_dentry();
    root->parent = root;
    root->file_system = fs;
    attach_inode(root, root_inode);
    fs->root = root;

    return fs;
}
