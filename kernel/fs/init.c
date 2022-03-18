#include <basic.h>
#include <ng/fs.h>
#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/file_system.h>
#include <ng/fs/inode.h>
#include <ng/fs/proc.h>
#include <ng/fs/tmpfs.h>
#include <string.h>

void proc2_test(struct file *file, void *arg)
{
    static int n = 0;
    proc2_sprintf(file, "Hello World %i", ++n);
}

void fs2_init(void *initfs)
{
    initfs_file_system = new_tmpfs_file_system();
    proc_file_system = new_tmpfs_file_system();

    global_root_dentry = new_dentry();
    global_root_dentry->name = strdup("");
    global_root_dentry->parent = global_root_dentry;

    mount_file_system(initfs_file_system, global_root_dentry);

    void load_initfs2(void *initfs);
    load_initfs2(initfs);
}
