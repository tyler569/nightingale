#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/file_system.h>
#include <ng/fs/init.h>
#include <ng/fs/tmpfs.h>
#include <ng/fs/vnode.h>
#include <ng/init.h>
#include <ng/limine.h>
#include <ng/proc_files.h>
#include <string.h>

void fs_init(void *initfs) {
	initfs_file_system = new_tmpfs_file_system();
	proc_file_system = new_tmpfs_file_system();

	global_root_dentry = new_dentry();
	global_root_dentry->name = strdup("");
	global_root_dentry->parent = global_root_dentry;

	mount_file_system(initfs_file_system, global_root_dentry);

	load_initfs(initfs);
}

void init_fs() {
	struct tar_header *initfs = limine_module();
	fs_init(initfs);
	procfs_init();
}
define_init(init_fs, 3);
