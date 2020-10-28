#pragma once
#ifndef NG_FS_H
#define NG_FS_H

#include <basic.h>
#include <ng/syscall.h>
#include <ng/syscall_consts.h>
#include <ng/dmgr.h>
#include <ng/ringbuf.h>
#include <ng/tty.h>
#include <list.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

typedef int64_t off_t;
typedef int nfds_t;

struct file;
struct open_file;
struct directory_file;

enum file_flags {
        FILE_NONBLOCKING = 0x01,
};

struct file_ops {
        void (*open)(struct open_file *n);
        void (*close)(struct open_file *n);
        ssize_t (*read)(struct open_file *n, void *data, size_t len);
        ssize_t (*write)(struct open_file *n, const void *data, size_t len);
        off_t (*seek)(struct open_file *n, off_t offset, int whence);
        void (*destroy)(struct file *);
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

        list blocked_threads;
};

struct open_file {
        struct file *node;
        int flags;
        off_t off;

        // only used in procfs for now
        char *buffer;
        off_t length;
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

struct file *fs_resolve_relative_path(struct file *root, const char *filename);
struct file *fs_path(const char *filename);
// void vfs_print_tree(struct file *root, int indent);

void destroy_file(struct file *);
sysret do_close_open_file(struct open_file *);


// directory

struct directory_file {
        struct file file;
        list directory_entries;
};

struct directory_node {
        struct file *file;
        const char *name;
        list directory_siblings;
};

extern struct file_ops directory_ops;

struct file *make_directory(struct file *directory, const char *name);
struct file *fs_root_init(void);
void add_dir_file(struct file *directory, struct file *file, const char *name);
struct file *dir_child(struct file *directory, const char *name);


// membuf

struct membuf_file {
        struct file file;
        void *memory;
        off_t capacity;
};

extern struct file_ops membuf_ops;

ssize_t membuf_read(struct open_file *n, void *data, size_t len);
ssize_t membuf_write(struct open_file *n, const void *data, size_t len);
off_t membuf_seek(struct open_file *n, off_t offset, int whence);
void membuf_close(struct open_file *n);

struct file *create_file(struct file *directory, const char *name, int mode);
struct file *make_tar_file(const char *filename, size_t len, void *data);


// tty

struct tty_file {
        struct file file;
        struct tty tty;
        // struct ringbuf *ring;
};

extern struct file_ops tty_ops;

ssize_t dev_serial_write(struct open_file *n, const void *data_, size_t len);
ssize_t dev_serial_read(struct open_file *n, void *data_, size_t len);


// pipe

struct pipe_file {
        struct file file;
        struct ringbuf ring;
};

extern struct file_ops pipe_ops;


// stuff ?

ssize_t dev_zero_read(struct open_file *n, void *data_, size_t len);
ssize_t dev_null_write(struct open_file *n, const void *data, size_t len);
ssize_t dev_inc_read(struct open_file *n, void *data_, size_t len);

#endif // NG_FS_H
