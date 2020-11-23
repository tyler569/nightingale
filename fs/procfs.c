#include <assert.h>
#include <basic.h>
#include <ng/fs.h>
#include <ng/thread.h>
#include <stdlib.h>

ssize_t proc_readdir(struct open_file *ofd, struct ng_dirent *buf,
                     size_t count) {
    struct file *file = ofd->node;
    if (file->filetype != FT_DIRECTORY) { return -ENOTDIR; }
    struct directory_file *directory = (struct directory_file *)file;

    int index = 0;

    // TODO: ".", ".." first.

    list_for_each(struct thread, th, &all_threads, all_threads) {
        if (index > count) return index;
        buf[index].type = FT_DIRECTORY;
        sprintf(&buf[index].filename[0], "%i\0", th->tid);
        index++;
    }

    list_for_each(struct directory_node, node, &directory->entries, siblings) {
        if (index > count) return index;
        buf[index].type = node->file->filetype;
        buf[index].permissions = node->file->permissions;
        strncpy(&buf[index].filename[0], node->name, 64);
        index++;
    }

    return index;
}

struct file *proc_child(struct file *directory, const char *name) {
    char *endptr;
    long tid = strtol(name, &endptr, 10);
    if (endptr[0] == 0) {
        struct thread *th = thread_by_id(tid);
        if (!th) return NULL;
        return &fs_root_node->file; // TODO make a thread_file
    }
    return directory_child(directory, name);
}

struct file_ops proc_ops = {
    .readdir = proc_readdir,
    .child = proc_child,
};

struct file *make_proc(struct file *directory) {
    struct file *new = make_directory(directory, "proc");
    new->ops = &proc_ops;
    return new;
}
