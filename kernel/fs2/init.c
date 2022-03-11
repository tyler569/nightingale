#include <basic.h>
#include <ng/fs.h>
#include <string.h>
#include "dentry.h"
#include "file.h"
#include "file_system.h"
#include "inode.h"
#include "proc.h"
#include "tmpfs.h"

void proc2_test(struct fs2_file *file, void *arg) {
    proc2_sprintf(file, "Hello World %i", 1);
}

void fs2_init(void *initfs) {
    initfs_file_system = new_tmpfs_file_system();
    proc_file_system = new_tmpfs_file_system();

    global_root_dentry = new_dentry();
    global_root_dentry->name = strdup("");
    global_root_dentry->parent = global_root_dentry;

    mount_file_system(initfs_file_system, global_root_dentry);

    void load_initfs2(void *initfs);
    load_initfs2(initfs);

    make_proc_file2("test", proc2_test, NULL);
}
