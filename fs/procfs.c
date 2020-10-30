#include <basic.h>
#include <ng/fs.h>
#include <ng/thread.h>
#include <assert.h>
#include <stdlib.h>

ssize_t procfs_thread_read(struct open_file *ofd, void *data, size_t len) {
        return 0;
}

ssize_t procfs_thread_readdir(struct open_file *ofd, struct ng_dirent *buf, size_t len) {
        return 0;
}

struct file_ops procfs_thread_ops = {
        .read = procfs_thread_read,
        .readdir = procfs_thread_readdir,
};

struct file *make_thread_procfile(struct thread *thread) {
        struct file *proc = fs_path("/proc");
        struct procfs_thread_file *procfile = zmalloc(sizeof(struct procfs_thread_file));

        char *filename = zmalloc(16);
        sprintf(filename, "%i", thread->tid);

        make_directory_inplace(proc, &procfile->file, filename);

        procfile->file.filetype = FT_PROC_THREAD;
        procfile->thread = thread;

        return &procfile->file;
}
