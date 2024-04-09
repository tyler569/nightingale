#include <list.h>
#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/file_system.h>
#include <ng/fs/vnode.h>
#include <ng/time.h>
#include <stdlib.h>

struct file_system_ops default_file_system_ops = { 0 };
struct file_system *initfs_file_system;
struct file_system *proc_file_system;

struct vnode *new_vnode_notime(struct file_system *file_system, int mode) {
	struct vnode *vnode;
	if (file_system->ops->new_vnode) {
		vnode = file_system->ops->new_vnode(file_system);
	} else {
		vnode = calloc(1, sizeof(struct vnode));
	}

	vnode->file_system = file_system;
	vnode->mode = mode;
	vnode->ops = &default_ops;
	vnode->file_ops = &default_file_ops;
	vnode->vnode_number = file_system->next_vnode_number++;

	vnode->type = mode >> 16 ? mode >> 16 : FT_NORMAL;

	wq_init(&vnode->read_queue);
	wq_init(&vnode->write_queue);

	list_append(&file_system->vnodes, &vnode->fs_vnodes);

	return vnode;
}

struct vnode *new_vnode(struct file_system *file_system, int mode) {
	struct vnode *vnode = new_vnode_notime(file_system, mode);
	vnode->atime = vnode->mtime = vnode->ctime = time_now();
	return vnode;
}

void mount_file_system(struct file_system *file_system, struct dentry *dentry) {
	dentry->mounted_file_system = file_system;
}
