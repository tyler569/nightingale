#include <basic.h>
#include <ng/fs.h>
#include <string.h>
#include "dentry.h"
#include "file.h"
#include "inode.h"

void fs2_init(void) {
    global_root_dentry = new_dentry();
    global_root_dentry->name = strdup("");
    global_root_dentry->parent = global_root_dentry;

    struct inode *global_root = new_inode(_NG_DIR, 0644);
    global_root_dentry->inode = global_root;
}
