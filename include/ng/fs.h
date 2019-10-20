
#ifndef NIGHTINGALE_FS_VFS_H
#define NIGHTINGALE_FS_VFS_H

#include <ng/basic.h>
#include <ng/syscall.h>
#include <ng/syscall_consts.h>
#include <ds/dmgr.h>
#include <ds/list.h>
#include <ds/ringbuf.h>
#include <ds/vector.h>
#include <ng/tty.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

enum filetype {
        CHAR_DEV,      // like /dev/null
        TTY,           // like /dev/serial
        MEMORY_BUFFER, // like initfs
        ON_DISK,
        NET_SOCK,
        DIRECTORY,
};

#define ALL_READ   0x0004
#define ALL_WRITE  0x0002
#define ALL_EXEC   0x0001
#define GRP_READ   0x0040
#define GRP_WRITE  0x0020
#define GRP_EXEC   0x0010
#define USR_READ   0x0400
#define USR_WRITE  0x0200
#define USR_EXEC   0x0100

#define SUID       0x1000
#define SGID       0x2000

typedef int64_t off_t;

struct fs_node;
struct open_fd;

struct fs_ops {
        int (*open)(struct fs_node *n);
        int (*close)(struct open_fd *n);
        ssize_t (*read)(struct open_fd *n, void *data, size_t len);
        ssize_t (*write)(struct open_fd *n, const void *data, size_t len);
        off_t (*seek)(struct open_fd *n, off_t offset, int whence);
};

enum file_flags {
        FILE_NONBLOCKING = 0x01,
};

struct fs_node {
        int filetype;
        char filename[256];
        atomic_int refcnt;

        int flags;
        int permission;
        int uid;
        int gid;

        off_t len;

        struct fs_ops ops;

        struct list blocked_threads;

        struct fs_node *parent;

        union {
                struct {
                        struct ringbuf ring;
                        struct tty *tty;
                };
                void *memory;
                uintptr_t handle;
                struct list children;
        } extra;
};

struct open_fd {
        struct fs_node *node;
        int flags;
        off_t off;
};

extern struct fs_node *dev_serial;
extern struct open_fd *ofd_stdin;
extern struct open_fd *ofd_stdout;
extern struct open_fd *ofd_stderr;

// seek

enum {
        SEEK_SET,
        SEEK_CUR,
        SEEK_END,
};

// poll

struct pollfd {
        int fd;
        short events;
        short revents;
};

enum {
        POLLIN,
};

typedef int nfds_t;

extern struct dmgr fs_node_table;
extern struct fs_node *fs_root_node;

void vfs_init();
void mount(struct fs_node *n, char *path);

struct fs_node *fs_resolve_relative_path(struct fs_node *root, const char *filename);
void vfs_print_tree(struct fs_node *root, int indent);

#endif
