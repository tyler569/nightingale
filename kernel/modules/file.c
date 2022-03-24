#include <basic.h>
#include <ng/fs.h>
#include <ng/mod.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <stdio.h>

ssize_t my_file_read(struct file *ofd, char *buf, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        ((char *)buf)[i] = (char)i;
    }
    return (ssize_t)len;
}

struct file_operations my_file_ops = {
    .read = my_file_read,
};

int make_my_file(const char *name)
{
    struct dentry *path = resolve_path(name);
    if (IS_ERROR(path)) {
        printf("error %li creating %s\n", -ERROR(path), name);
        return MODINIT_FAILURE;
    }
    if (dentry_inode(path)) {
        printf("error creating %s: already exists\n", name);
        return MODINIT_FAILURE;
    }

    struct inode *inode = new_inode(path->file_system, 0444);
    inode->type = FT_CHAR_DEV;
    inode->file_ops = &my_file_ops;
    attach_inode(path, inode);
    return MODINIT_SUCCESS;
}

int init(struct mod *_)
{
    printf("Hello World from this kernel module!\n");
    return make_my_file("/dev/modfile");
}

__USED struct modinfo modinfo = {
    .name = "file",
    .init = init,
};
