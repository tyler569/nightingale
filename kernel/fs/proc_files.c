#include <ng/fs.h>
#include <ng/init.h>
#include <ng/proc_files.h>

static void proc_test(struct file *ofd, void *) {
	proc_sprintf(ofd, "Hello World\n");
}
define_proc_file("test", proc_test, nullptr);

extern struct proc_file_def proc_files_start[], proc_files_end[];

void procfs_init() {
	for (struct proc_file_def *c = proc_files_start; c < proc_files_end; c++) {
		make_proc_file(c->name, c->fn, c->ctx);
	}
}
