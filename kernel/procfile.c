
#include <basic.h>
#include <ng/fs.h>
#include <nc/assert.h>
#include <nc/stdlib.h>
#include <ng/procfile.h>

ssize_t procfs_read(struct open_file *ofd, void *data, size_t len) {
        const ssize_t in_buffer = ofd->length - ofd->off;
        const ssize_t to_read = min(len, in_buffer);

        if (to_read > 0) {
                memcpy(data, ofd->buffer + ofd->off, to_read);
        }

        ofd->off += to_read;
        return to_read;
}

void procfs_close(struct open_file *ofd) {
        free(ofd->buffer);
}

struct file *make_procfile(const char *name,
                void (*fn)(struct open_file *), void *data) {
        struct file *proc = fs_resolve_relative_path(NULL, "/proc");
        assert(proc && proc->filetype == FT_DIRECTORY);

        struct file *procfile = zmalloc(sizeof(struct file));
        procfile->filetype = FT_PROC;
        strcpy(procfile->filename, name);
        procfile->permissions = USR_READ;
        procfile->open = fn;
        procfile->read = procfs_read;
        procfile->close = procfs_close;
        procfile->memory = data;
        list_init(&procfile->blocked_threads);

        put_file_in_dir(procfile, proc);
        return procfile;
}

// This makes the procfiles that exist at boot
void procfs_init() {
        extern void proc_test(struct open_file *);
        extern void pmm_procfile(struct open_file *);
        // extern void malloc_procfile(struct open_file *);
        // extern void malloc_detail_procfile(struct open_file *);
        extern void timer_procfile(struct open_file *);
        make_procfile("test", proc_test, NULL);
        make_procfile("pmm", pmm_procfile, NULL);
        // make_procfile("malloc", malloc_procfile, NULL);
        // make_procfile("malloc_detail", malloc_detail_procfile, NULL);
        make_procfile("timer", timer_procfile, NULL);
}

