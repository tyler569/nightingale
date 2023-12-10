#include <ng/fs.h>
#include <ng/proc_files.h>

static void proc_test(file *ofd, void *) { proc_sprintf(ofd, "Hello World\n"); }

extern "C" {
void timer_procfile(file *, void *);
void proc_syscalls(file *, void *);
void proc_mods(file *, void *);
void pm_summary(file *, void *);
void proc_heap(file *, void *);
}

void procfs_init(void)
{
    make_proc_file("test", proc_test, nullptr);
    make_proc_file("timer", timer_procfile, nullptr);
    make_proc_file("mem", pm_summary, nullptr);
    make_proc_file("syscalls", proc_syscalls, nullptr);
    make_proc_file("mods", proc_mods, nullptr);
    make_proc_file("heap", proc_heap, nullptr);

    dentry *ddir = proc_file_system->root;
    extern inode_operations proc_self_ops;
    inode *inode = new_inode(proc_file_system, _NG_SYMLINK | 0444);
    inode->ops = &proc_self_ops;
    add_child(ddir, "self", inode);
}
