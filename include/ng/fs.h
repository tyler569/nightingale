#pragma once
#ifndef NG_FS_H
#define NG_FS_H

#include <basic.h>
#include <ng/dmgr.h>
#include <ng/ringbuf.h>
#include <ng/sync.h>
#include <ng/syscall.h>
#include <ng/syscall_consts.h>
#include <ng/tty.h>
#include <dirent.h>
#include <list.h>
#include <poll.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

struct file;
struct open_file;
struct directory_file;
struct thread;

enum file_flags {
    FILE_NONBLOCKING = 0x01,
};

struct file_ops {
    void (*open)(struct open_file *, const char *name);
    void (*close)(struct open_file *);
    void (*destroy)(struct file *);
    ssize_t (*read)(struct open_file *, void *, size_t);
    ssize_t (*write)(struct open_file *, const void *, size_t);
    off_t (*seek)(struct open_file *, off_t, int whence);
    ssize_t (*readdir)(
        struct open_file *,
        struct ng_dirent *,
        size_t
    );
    void (*clone)(
        struct open_file *parent,
        struct open_file *child
    );
    struct file *(*child)(struct file *, const char *name);
};

struct file {
    enum file_type type;
    enum file_flags flags;
    enum file_mode mode;

    atomic_int refcnt;
    int signal_eof;
    int uid;
    int gid;
    off_t len;
    struct file_ops *ops;

    waitqueue_t readq;
    waitqueue_t writeq;
};

struct open_file {
    struct file *file;
    enum file_mode mode;
    off_t off;

    // only used in procfs for now
    char *buffer;
    off_t buffer_size;         // total size
    off_t buffer_length;       // in use
};

// poll

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
sysret do_open(struct file *, const char *basename, int flags, int mode);

struct open_file *get_file1(int fd);

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

ssize_t directory_readdir(
    struct open_file *ofd,
    struct ng_dirent *buf,
    size_t count
);
struct file *make_directory(struct file *directory, const char *name);
struct file *make_directory_inplace(
    struct file *directory,
    struct file *new,
    const char *name
);
struct file *fs_root_init(void);
sysret add_dir_file(
    struct file *directory,
    struct file *file,
    const char *name
);
struct file *remove_dir_file(struct file *directory, const char *name);
struct file *directory_child(struct file *directory, const char *name);
void directory_destroy(struct file *directory);


struct file *make_procdir(struct file *directory);

// membuf

struct membuf_file {
    struct file file;
    void *memory;
    off_t capacity;
};

struct file *create_file(struct file *directory, const char *name, int mode);
struct file *make_tar_file(
    const char *filename,
    int perm,
    size_t len,
    void *data
);

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

void make_procfile(
    const char *name,
    void (*generate)(struct open_file *ofd, void *arg),
    void *argument
);
void proc_sprintf(struct open_file *ofd, const char *format, ...)
__PRINTF(2, 3);

// socket

struct socket_file {
    struct file file;
    struct socket_ops *ops;
    enum socket_mode mode;
    enum socket_domain domain;
    enum socket_type type;
    enum socket_protocol protocol;
};

struct socket_ops {
    struct socket_file *(*alloc)(void);
    void (*init)(struct socket_file *);
    int (*bind)(
        struct socket_file *,
        const struct sockaddr *,
        socklen_t
    );
    ssize_t (*recv)(
        struct open_file *,
        void *,
        size_t,
        int flags
    );
    ssize_t (*send)(
        struct open_file *,
        const void *,
        size_t,
        int flags
    );
    ssize_t (*recvfrom)(
        struct open_file *,
        void *,
        size_t,
        int flags,
        struct sockaddr *,
        socklen_t *
    );
    ssize_t (*sendto)(
        struct open_file *,
        const void *,
        size_t,
        int flags,
        const struct sockaddr *,
        socklen_t
    );
    int (*listen)(struct open_file *, int backlog);
    int (*accept)(struct open_file *, struct sockaddr *, socklen_t *);
    int (*connect)(
        struct open_file *,
        const struct sockaddr *,
        socklen_t
    );
    void (*close)(struct open_file *);
};

// stuff

ssize_t dev_zero_read(struct open_file *n, void *data, size_t len);
ssize_t dev_null_read(struct open_file *n, void *data, size_t len);
ssize_t dev_null_write(struct open_file *n, const void *data, size_t len);
ssize_t dev_inc_read(struct open_file *n, void *data_, size_t len);

ssize_t dev_random_read(struct open_file *n, void *data, size_t len);
ssize_t dev_random_write(struct open_file *n, const void *data, size_t len);

ssize_t dev_count_read(struct open_file *n, void *data, size_t len);

// #include "../../kernel/fs2/file.h"
// #include "../../kernel/fs2/inode.h"
// #include "../../kernel/fs2/file_system.h"
// #include "../../kernel/fs2/dentry.h"

#endif // NG_FS_H
