
#ifndef NIGHTINGALE_FS_VFS_H
#define NIGHTINGALE_FS_VFS_H

#include <ng/basic.h>
#include <ng/syscall.h>
#include <ng/syscall_consts.h>
#include <ds/dmgr.h>
#include <ds/list.h>
#include <ds/ringbuf.h>
#include <ds/vector.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

enum filetype {
        CHAR_DEV,      // like /dev/null
        PTY,           // like /dev/serial
        MEMORY_BUFFER, // like initfs
        ON_DISK,
        NET_SOCK,
        DIRECTORY,
};

#define ALL_READ 00004
#define ALL_WRITE 00002
#define ALL_EXEC 00001
#define GRP_READ 00040
#define GRP_WRITE 00020
#define GRP_EXEC 00010
#define USR_READ 00400
#define USR_WRITE 00200
#define USR_EXEC 00100

#define SUID 01000
#define SGID 02000

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
                struct ringbuf ring;
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
extern struct open_fd *dev_stdin;
extern struct open_fd *dev_stdout;
extern struct open_fd *dev_stderr;

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

struct fs_node *get_file_by_name(struct fs_node *root, char *filename);
void vfs_print_tree(struct fs_node *root, int indent);

#endif
