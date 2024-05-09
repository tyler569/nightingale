#include <ng/fs.h>
#include <ng/mod.h>
#include <stdio.h>

ssize_t my_file_read(struct file *ofd, char *buf, size_t len) {
	for (size_t i = 0; i < len; i++) {
		((char *)buf)[i] = (char)i;
	}
	return (ssize_t)len;
}

struct file_ops my_file_ops = {
	.read = my_file_read,
};

int make_my_file(const char *name) {
	struct dentry *path = resolve_path(name);
	if (IS_ERROR(path)) {
		printf("error %li creating %s\n", -ERROR(path), name);
		return MODINIT_FAILURE;
	}
	if (dentry_vnode(path)) {
		printf("error creating %s: already exists\n", name);
		return MODINIT_FAILURE;
	}

	struct vnode *vnode = new_vnode(path->file_system, 0444);
	vnode->type = FT_CHAR_DEV;
	vnode->file_ops = &my_file_ops;
	attach_vnode(path, vnode);
	return MODINIT_SUCCESS;
}

int init(struct mod *) {
	printf("Hello World from this kernel module!\n");
	return make_my_file("/dev/modfile");
}

__USED struct modinfo modinfo = {
	.name = "file",
	.init = init,
};
