#include <list.h>
#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/file_system.h>
#include <ng/fs/inode.h>
#include <ng/time.h>
#include <stdlib.h>

file_system_operations default_file_system_ops = {};
file_system *initfs_file_system;
file_system *proc_file_system;

inode *new_inode_notime(file_system *file_system, int mode)
{
    inode *i;
    if (file_system->ops->new_inode) {
        i = file_system->ops->new_inode(file_system);
    } else {
        i = (inode *)calloc(1, sizeof(inode));
    }

    i->file_system = file_system;
    i->mode = mode;
    i->ops = &default_ops;
    i->file_ops = &default_file_ops;
    i->inode_number = file_system->next_inode_number++;

    i->type = static_cast<file_type>(mode >> 16 ? mode >> 16 : FT_NORMAL);

    wq_init(&i->read_queue);
    wq_init(&i->write_queue);

    list_append(&file_system->inodes, &i->fs_inodes);

    return i;
}

inode *new_inode(file_system *file_system, int mode)
{
    inode *inode = new_inode_notime(file_system, mode);
    inode->atime = inode->mtime = inode->ctime = time_now();
    return inode;
}

void mount_file_system(file_system *file_system, dentry *dentry)
{
    dentry->mounted_file_system = file_system;
}
