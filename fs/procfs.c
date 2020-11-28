#include <basic.h>
#include <assert.h>
#include <ng/fs.h>
#include <ng/thread.h>
#include <stdlib.h>

struct thread_file;
struct file *make_thread_file(struct thread *th);

//  proc directory ----------------------------------------------------------

ssize_t procdir_readdir(struct open_file *ofd, struct ng_dirent *buf,
                        size_t count) {
    struct file *file = ofd->node;
    if (file->filetype != FT_DIRECTORY) { return -ENOTDIR; }
    struct directory_file *directory = (struct directory_file *)file;

    int index = 0;

    // TODO: ".", ".." first.

    list_for_each(struct thread, th, &all_threads, all_threads) {
        if (index > count) return index;
        buf[index].type = FT_DIRECTORY;
        sprintf(buf[index].filename, "%i\0", th->tid);
        index++;
    }

    list_for_each(struct directory_node, node, &directory->entries, siblings) {
        if (index > count) return index;
        buf[index].type = node->file->filetype;
        buf[index].permissions = node->file->permissions;
        strncpy(buf[index].filename, node->name, 64);
        index++;
    }

    return index;
}

struct file *procdir_child(struct file *directory, const char *name) {
    char *endptr;
    long tid = strtol(name, &endptr, 10);
    if (endptr[0] == 0) {
        struct thread *th = thread_by_id(tid);
        if (!th) return NULL;
        return make_thread_file(th);
    }
    return directory_child(directory, name);
}

struct file_ops procdir_ops = {
    .readdir = procdir_readdir,
    .child = procdir_child,
};

struct file *make_procdir(struct file *directory) {
    struct file *new = make_directory(directory, "proc");
    new->ops = &procdir_ops;
    return new;
}

//  thread file     ----------------------------------------------------------

struct thread_file {
    struct file file;
    struct thread *thread;
};

struct file_ops thread_dir_ops;

struct file *make_thread_file(struct thread *th) {
    struct thread_file *thread_file = zmalloc(sizeof(struct thread_file));
    thread_file->file.filetype = FT_PROC_THREAD;
    thread_file->file.refcnt = 1;
    thread_file->file.ops = &thread_dir_ops;
    thread_file->file.permissions = USR_READ | USR_EXEC;
    thread_file->thread = th;

    return &thread_file->file;
}

const char *thread_proc_names[] = {
    "comm",
    "pid",
};

ssize_t thread_dir_readdir(struct open_file *ofd, struct ng_dirent *buf,
                           size_t count) {
    struct file *file = ofd->node;
    assert(file->filetype == FT_PROC_THREAD);
    struct thread_file *thread = (struct thread_file *)file;

    int index = 0;
    buf[index].type = FT_DIRECTORY;
    buf[index].permissions = USR_READ | USR_EXEC;
    strcpy(buf[index].filename, ".");
    index++;
    if (index > count) return index;
    buf[index].type = FT_DIRECTORY;
    buf[index].permissions = USR_READ | USR_EXEC;
    strcpy(buf[index].filename, "..");
    index++;
    if (index > count) return index;
    for (int i = 0; i < ARRAY_LEN(thread_proc_names); i++) {
        buf[index].type = FT_PROC_THREAD;
        buf[index].permissions = USR_READ;
        strcpy(buf[index].filename, thread_proc_names[i]);
        index++;
        if (index > count) return index;
    }
    return index;
}

struct file *thread_dir_child(struct file *file, const char *name) {
    if (strcmp(name, ".") == 0) { return file; }
    if (strcmp(name, "..") == 0) { return fs_path("/proc"); }
    return NULL;
}

struct file_ops thread_dir_ops = {
    .readdir = thread_dir_readdir,
    .child = thread_dir_child,
};

struct file_ops thread_file_ops = {
    0
    // .open = thread_open,
    // .read = thread_read,
};

//  procedure file  ----------------------------------------------------------

struct proc_file {
    struct file file;
    void (*generate)(struct open_file *ofd, void *arg);
    void *argument;
};

void proc_sprintf(struct open_file *ofd, const char *format, ...) {
    va_list args;
    va_start(args, format);

    ofd->buffer_length +=
        vsnprintf(ofd->buffer + ofd->buffer_length,
                  ofd->buffer_size - ofd->buffer_length, format, args);
    // va_end is called in vsprintf
}

void proc_open(struct open_file *ofd, const char *_name) {
    assert(ofd->node->filetype == FT_PROC);
    struct proc_file *proc = (struct proc_file *)ofd->node;
    assert(proc->generate);
    ofd->buffer = malloc(8192);
    ofd->buffer_size = 8192;
    ofd->buffer_length = 0;

    proc->generate(ofd, proc->argument);
}

void proc_close(struct open_file *ofd) {
    free(ofd->buffer);
}

void proc_clone(struct open_file *parent, struct open_file *child) {
    child->buffer = malloc(parent->buffer_size);
    memcpy(child->buffer, parent->buffer, parent->buffer_size);
}

ssize_t proc_read(struct open_file *ofd, void *buffer, size_t len) {
    // I think this is actually right -- `min` and `max` are signed
    // in my implementation, so they'll cast to `intptr_t` and then
    // the clamp to max(x, 0) will actually work if it does negative.
    size_t to_read = max(min(len, ofd->buffer_length - ofd->off), 0);

    memcpy(buffer, ofd->buffer + ofd->off, to_read);
    ofd->off += to_read;
    return to_read;
}

struct file_ops proc_ops = {
    .open = proc_open,
    .close = proc_close,
    .clone = proc_clone,
    .read = proc_read,
    // .write = proc_write,
};

void make_procfile(const char *name,
                   void (*generate)(struct open_file *ofd, void *arg),
                   void *argument) {
    struct file *procdir_file = fs_path("/proc");
    assert(procdir_file && procdir_file->filetype == FT_DIRECTORY);
    struct directory_file *procdir = (struct directory_file *)procdir_file;

    struct proc_file *proc = zmalloc(sizeof(struct proc_file));
    proc->file.filetype = FT_PROC;
    proc->file.permissions = USR_READ; // TODO: writeable procfiles
    proc->file.refcnt = 1;
    proc->file.ops = &proc_ops;
    wq_init(&proc->file.wq);

    proc->generate = generate;
    proc->argument = argument;

    add_dir_file(procdir_file, &proc->file, name);
}
