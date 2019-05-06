
#ifndef NIGHTINGALE_FS_VFS_H
#define NIGHTINGALE_FS_VFS_H

#include <ng/basic.h>
#include <ng/syscall.h>
#include <ds/dmgr.h>
#include <ds/list.h>
#include <ds/ringbuf.h>
#include <ds/vector.h>
#include <stddef.h>
#include <stdint.h>

enum filetype {
        CHAR_DEV,      // like /dev/null
        PTY,           // like /dev/serial
        MEMORY_BUFFER, // like initfs
        ON_DISK,
        NET_SOCK,
        DIRECTORY,
        MOUNTPOINT,
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

struct fs_ops {
        int (*open)(struct fs_node *n);
        int (*close)(struct fs_node *n);
        ssize_t (*read)(struct fs_node *n, void *data, size_t len);
        ssize_t (*write)(struct fs_node *n, const void *data, size_t len);
        off_t (*seek)(struct fs_node *n, off_t offset, int whence);
};

enum file_flags {
        FILE_NONBLOCKING = 0x01,
};

struct fs_node {
        int filetype;
        char *filename;

        int flags;
        int permission;
        int uid;
        int gid;

        off_t len;
        off_t off;

        struct fs_ops ops;

        struct list blocked_threads;

        union {
                struct ringbuf ring;
                void *memory;
                uintptr_t handle;
                struct list children;
        } extra;
};

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

#define O_RDONLY 0x01

typedef int nfds_t;

struct dmgr fs_node_table;

void vfs_init();
void mount(struct fs_node *n, char *path);

struct syscall_ret sys_open(const char *filename, int flags);
struct syscall_ret sys_read(int fd, void *data, size_t len);
struct syscall_ret sys_write(int fd, const void *data, size_t len);
struct syscall_ret sys_dup2(int oldfd, int newfd);
struct syscall_ret sys_seek(int fs, off_t offset, int whence);

#endif
