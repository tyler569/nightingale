#include <basic.h>
#include <string.h>
#include "dentry.h"
#include "file.h"
#include "inode.h"
#include "tmpfs.h"

void fs2_init(void *initfs) {
    initfs_file_system = new_tmpfs_file_system();

    global_root_dentry = new_dentry();
    global_root_dentry->name = strdup("");
    global_root_dentry->parent = global_root_dentry;

    struct inode *global_root = new_inode(initfs_file_system, _NG_DIR | 0644);
    global_root->inode_number = 2;
    attach_inode(global_root_dentry, global_root);

    initfs_file_system->root_inode = global_root;
    initfs_file_system->mounted_on = global_root_dentry;
    global_root_dentry->file_system = initfs_file_system;

    void load_initfs2(void *initfs);
    load_initfs2(initfs);
}
