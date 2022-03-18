#include <basic.h>
#include <ng/fs.h>
#include <ng/mod.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <stdio.h>

ssize_t my_file_read(struct fs2_file *ofd, char *buf, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        ((char *)buf)[i] = (char)i;
    }
    return (ssize_t)len;
}

struct file_operations my_file_ops = {
    .read = my_file_read,
};

void make_my_file(const char *name)
{
    struct dentry *dir = resolve_path("/dev");
    if (IS_ERROR(dir)) {
        printf("failed to create file because /dev does not exist\n");
        return;
    }
    struct dentry *path = resolve_path_from(dir, name, true);
    if (dentry_inode(path)) {
        printf("failed to create file becasue /dev/%s already exists\n", name);
        return;
    }

    struct inode *inode = new_inode(dir->file_system, 0444);
    inode->file_ops = &my_file_ops;
    attach_inode(path, inode);
}

int init(struct mod *_)
{
    printf("Hello World from this kernel module!\n");
    make_my_file("modfile");
    return MODINIT_SUCCESS;
}

__USED struct modinfo modinfo = {
    .name = "file",
    .init = init,
};
