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
    
    {
        proc_file_system = new_tmpfs_file_system();
        struct dentry *proc_root_dentry = new_dentry();
        proc_file_system->root_dentry = proc_root_dentry;
        make_proc_file2("test", proc2_test, NULL);
    }

    global_root_dentry = new_dentry();
    global_root_dentry->name = strdup("");
    global_root_dentry->parent = global_root_dentry;

    struct inode *global_root = new_inode(initfs_file_system, _NG_DIR | 0644);
    global_root->inode_number = 2;
    attach_inode(global_root_dentry, global_root);

    initfs_file_system->root_inode = global_root;
    initfs_file_system->mounted_on = global_root_dentry;
    initfs_file_system->root_dentry = global_root_dentry;
    global_root_dentry->file_system = initfs_file_system;

    void load_initfs2(void *initfs);
    load_initfs2(initfs);
}
