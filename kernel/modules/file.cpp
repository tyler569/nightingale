#include <ng/common.h>
#include <ng/fs.h>
#include <ng/mod.h>
#include <stdio.h>

ssize_t my_file_read(file *ofd, char *buf, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        ((char *)buf)[i] = (char)i;
    }
    return (ssize_t)len;
}

file_operations my_file_ops = {
    .read = my_file_read,
};

int make_my_file(const char *name)
{
    dentry *path = resolve_path(name);
    if (IS_ERROR(path)) {
        printf("error %li creating %s\n", -ERROR(path), name);
        return MODINIT_FAILURE;
    }
    if (dentry_inode(path)) {
        printf("error creating %s: already exists\n", name);
        return MODINIT_FAILURE;
    }

    inode *inode = new_inode(path->file_system, 0444);
    inode->type = FT_CHAR_DEV;
    inode->file_ops = &my_file_ops;
    attach_inode(path, inode);
    return MODINIT_SUCCESS;
}

int init(mod *)
{
    printf("Hello World from this kernel module!\n");
    return make_my_file("/dev/modfile");
}

__USED modinfo modinfo = {
    .name = "file",
    .init = init,
};
