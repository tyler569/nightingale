#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/file_system.h>
#include <ng/fs/inode.h>
#include <ng/time.h>
#include <stdlib.h>
#include <list.h>

struct file_system_operations default_file_system_ops = { 0 };
struct file_system *initfs_file_system;
struct file_system *proc_file_system;

struct inode *new_inode_notime(struct file_system *file_system, int mode)
{
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
    inode->inode_number = file_system->next_inode_number++;

    inode->type = mode >> 16 ? mode >> 16 : FT_NORMAL;

    wq_init(&inode->read_queue);
    wq_init(&inode->write_queue);

    list_append(&file_system->inodes, &inode->fs_inodes);

    return inode;
}

struct inode *new_inode(struct file_system *file_system, int mode)
{
    struct inode *inode = new_inode_notime(file_system, mode);
    inode->atime = inode->mtime = inode->ctime = time_now();
    return inode;
}

void mount_file_system(struct file_system *file_system, struct dentry *dentry)
{
    dentry->mounted_file_system = file_system;
}
