#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/vnode.h>
#include <stdlib.h>

struct vnode_ops default_ops = { 0 };

static int close_file_refcounts(struct file *file);

static int open_file_refcounts(struct file *file) {
	// TODO: should the dentry refcounts be managed by vnode.c?
	if (file->dentry)
		atomic_fetch_add(&file->dentry->file_refcnt, 1);

	if (file->flags & O_RDONLY)
		atomic_fetch_add(&file->vnode->read_refcnt, 1);
	if (file->flags & O_WRONLY)
		atomic_fetch_add(&file->vnode->write_refcnt, 1);
	return 0;
}

int open_file_clone(struct file *file) {
	return open_file_refcounts(file);
}

int open_file(struct file *file) {
	int err = 0;
	open_file_refcounts(file);

	if (file->vnode->ops->open)
		err = file->vnode->ops->open(file->vnode, file);

	access_vnode(file->vnode);

	// What should the "open fails" pattern be?
	// if (IS_ERROR(err))
	//     close_file_refcounts(file);

	return err;
}

static int close_file_refcounts(struct file *file) {
	// TODO: should the dentry refcounts be managed by vnode.c?
	if (file->dentry)
		atomic_fetch_sub(&file->dentry->file_refcnt, 1);

	if (file->flags & O_RDONLY)
		atomic_fetch_sub(&file->vnode->read_refcnt, 1);
	if (file->flags & O_WRONLY)
		atomic_fetch_sub(&file->vnode->write_refcnt, 1);

	return 0;
}

int close_file(struct file *file) {
	int err = 0;
	close_file_refcounts(file);

	if (file->vnode->ops->close)
		err = file->vnode->ops->close(file->vnode, file);

	if (file->dentry)
		maybe_delete_dentry(file->dentry);
	else
		maybe_delete_vnode(file->vnode);

	return err;
}

void maybe_delete_vnode(struct vnode *vnode) {
	if (vnode->dentry_refcnt)
		return;
	if (vnode->read_refcnt)
		return;
	if (vnode->write_refcnt)
		return;

	// vnode->delete() that writes back to disk etc.?

	if (vnode->symlink_destination)
		free((void *)vnode->symlink_destination);
	list_remove(&vnode->fs_vnodes);
	// #include <stdio.h>
	//  printf("vnode (%c) freed\n", __filetype_sigils[vnode->type]);
	free(vnode);
}
