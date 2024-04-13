#include <ng/fs.h>
#include <ng/proc_files.h>

static void proc_test(struct file *ofd, void *) {
	proc_sprintf(ofd, "Hello World\n");
}

void procfs_init() {
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
}
