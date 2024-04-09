#include <ng/fs.h>
#include <ng/proc_files.h>

static void proc_test(struct file *ofd, void *) {
	proc_sprintf(ofd, "Hello World\n");
}

void procfs_init(void) {
	extern void timer_procfile(struct file *, void *);
	extern void proc_syscalls(struct file *, void *);
	extern void proc_mods(struct file *, void *);
	extern void pm_summary(struct file *, void *);
	extern void proc_heap(struct file * file, void *);

	make_proc_file("test", proc_test, nullptr);
	make_proc_file("timer", timer_procfile, nullptr);
	make_proc_file("mem", pm_summary, nullptr);
	make_proc_file("syscalls", proc_syscalls, nullptr);
	make_proc_file("mods", proc_mods, nullptr);
	make_proc_file("heap", proc_heap, nullptr);

	struct dentry *ddir = proc_file_system->root;
	extern struct vnode_ops proc_self_ops;
	struct vnode *vnode = new_vnode(proc_file_system, _NG_SYMLINK | 0444);
	vnode->ops = &proc_self_ops;
	add_child(ddir, "self", vnode);
}
