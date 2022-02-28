#include <basic.h>
#include <ng/fs.h>
#include "dentry.h"
#include "file.h"
#include "inode.h"

void fs2_init(void) {
    struct inode *global_root = new_inode(0644);
    global_root->type = FT_DIRECTORY;
    global_root->flags = IS_DIRECTORY;

    global_root_dentry->inode = global_root;
}
