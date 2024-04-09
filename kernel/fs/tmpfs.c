#include <ng/fs/dentry.h>
#include <ng/fs/file_system.h>
#include <ng/fs/tmpfs.h>
#include <ng/fs/vnode.h>
#include <stdlib.h>

struct file_system *new_tmpfs_file_system(void) {
	struct file_system *file_system = zmalloc(sizeof(struct file_system));
	file_system->ops = &default_file_system_ops;
	file_system->next_vnode_number = 2;
	list_init(&file_system->vnodes);

	struct vnode *root_vnode = new_vnode(file_system, _NG_DIR | 0755);
	struct dentry *root = new_dentry();
	root->parent = root;
	root->file_system = file_system;
	attach_vnode(root, root_vnode);
	file_system->root = root;

	return file_system;
}
