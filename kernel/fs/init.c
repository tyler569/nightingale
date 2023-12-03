#include "ng/fs/init.h"
#include "ng/fs/dentry.h"
#include "ng/fs/file.h"
#include "ng/fs/file_system.h"
#include "ng/fs/inode.h"
#include "ng/fs/tmpfs.h"
#include <string.h>

void fs_init(void *initfs)
{
    initfs_file_system = new_tmpfs_file_system();
    proc_file_system = new_tmpfs_file_system();

    global_root_dentry = new_dentry();
    global_root_dentry->name = strdup("");
    global_root_dentry->parent = global_root_dentry;

    mount_file_system(initfs_file_system, global_root_dentry);

    load_initfs(initfs);
}
