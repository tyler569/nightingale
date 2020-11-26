#pragma once
#ifndef NG_FS_H
#define NG_FS_H

#include <basic.h>
#include <dirent.h>
#include <list.h>
#include <ng/dmgr.h>
#include <ng/ringbuf.h>
#include <ng/sync.h>
#include <ng/syscall.h>
#include <ng/syscall_consts.h>
#include <ng/tty.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef int64_t off_t;
typedef int nfds_t;

struct file;
struct open_file;
struct directory_file;
struct thread;

enum file_flags {
    FILE_NONBLOCKING = 0x01,
};

struct file_ops {
    void (*open)(struct open_file *n);
    void (*close)(struct open_file *n);
    ssize_t (*read)(struct open_file *n, void *data, size_t len);
    ssize_t (*write)(struct open_file *n, const void *data, size_t len);
    off_t (*seek)(struct open_file *n, off_t offset, int whence);
    ssize_t (*readdir)(struct open_file *n, struct ng_dirent *buf,
                       size_t count);
    void (*clone)(struct open_file *parent, struct open_file *child);
    void (*destroy)(struct file *);
    struct file *(*child)(struct file *, const char *name);
};

struct file {
    enum filetype filetype;
    enum file_flags flags;
    enum file_permission permissions;

    atomic_int refcnt;
    int signal_eof;
    int uid;
    int gid;
    off_t len;
    struct file_ops *ops;

    struct wq wq;
};

struct open_file {
    struct file *node;
    int flags;
    off_t off;

    char *basename;

    // only used in procfs for now
    char *buffer;
    off_t buffer_size;   // total size
    off_t buffer_length; // in use
};

// poll

struct pollfd {
    int fd;
    short events;
    short revents;
};

enum poll_type {
    POLLIN,
};

extern struct directory_file *fs_root_node;

void vfs_init();
// void mount(struct file *n, char *path);
void put_file_in_dir(struct file *child, struct file *directory);
struct open_file *clone_open_file(struct open_file *ofd);
struct file *fs_resolve_relative_path(struct file *root, const char *filename);
struct file *fs_resolve_directory_of(struct file *root, const char *filename);
struct file *fs_path(const char *filename);
// void vfs_print_tree(struct file *root, int indent);

void destroy_file(struct file *);
sysret do_close_open_file(struct open_file *);
const char *basename(const char *path);

// directory

struct directory_file {
    struct file file;
    list entries;
};

struct directory_node {
    struct file *file;
    const char *name;
    list siblings;
};

extern struct file_ops directory_ops;

ssize_t directory_readdir(struct open_file *ofd, struct ng_dirent *buf,
                          size_t count);
struct file *make_directory(struct file *directory, const char *name);
struct file *make_directory_inplace(struct file *directory, struct file *new,
                                    const char *name);
struct file *fs_root_init(void);
void add_dir_file(struct file *directory, struct file *file, const char *name);
struct file *directory_child(struct file *directory, const char *name);
void directory_destroy(struct file *directory);

void remove_dir_child(struct file *directory, const char *name);
void remove_dir_child_file(struct file *directory, struct file *child);

struct file *make_procdir(struct file *directory);

// membuf

struct membuf_file {
    struct file file;
    void *memory;
    off_t capacity;
};

struct file *create_file(struct file *directory, const char *name, int mode);
struct file *make_tar_file(const char *filename, size_t len, void *data);

// tty

struct tty_file {
    struct file file;
    struct tty tty;
};

// extern struct file_ops tty_ops;
ssize_t dev_serial_write(struct open_file *n, const void *data_, size_t len);
ssize_t dev_serial_read(struct open_file *n, void *data_, size_t len);

// pipe

// procfs

void make_procfile(const char *name,
                   void (*generate)(struct open_file *ofd, void *arg),
                   void *argument);
void proc_sprintf(struct open_file *ofd, const char *format, ...);

// stuff ?

ssize_t dev_zero_read(struct open_file *n, void *data, size_t len);
ssize_t dev_null_read(struct open_file *n, void *data, size_t len);
ssize_t dev_null_write(struct open_file *n, const void *data, size_t len);
ssize_t dev_inc_read(struct open_file *n, void *data_, size_t len);

#endif // NG_FS_H
