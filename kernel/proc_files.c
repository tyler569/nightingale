#include "ng/proc_files.h"
#include "ng/fs.h"

static void proc_test(struct file *ofd, void *_)
{
    proc_sprintf(ofd, "Hello World\n");
}

void procfs_init(void)
{
    extern void timer_procfile(struct file *, void *);
    extern void proc_syscalls(struct file *, void *);
    extern void proc_mods(struct file *, void *);
    extern void pm_summary(struct file *, void *);
    extern void proc_heap(struct file * file, void *_);

    make_proc_file("test", proc_test, NULL);
    make_proc_file("timer", timer_procfile, NULL);
    make_proc_file("mem", pm_summary, NULL);
    make_proc_file("syscalls", proc_syscalls, NULL);
    make_proc_file("mods", proc_mods, NULL);
    make_proc_file("heap", proc_heap, NULL);

    struct dentry *ddir = proc_file_system->root;
    extern struct inode_operations proc_self_ops;
    struct inode *inode = new_inode(proc_file_system, _NG_SYMLINK | 0444);
    inode->ops = &proc_self_ops;
    add_child(ddir, "self", inode);
}
