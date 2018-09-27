
#ifndef NIGHTINGALE_FS_VFS_H
#define NIGHTINGALE_FS_VFS_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <vector.h>
#include <syscall.h>
#include <ringbuf.h>

enum filetype {
    CHAR_DEV,           // like /dev/null
    PTY,                // like /dev/serial
    MEMORY_BUFFER,      // like initfs
    ON_DISK,
    NET_SOCK,

    DIRECTORY,
    MOUNTPOINT,
};

#define ALL_READ  00004
#define ALL_WRITE 00002
#define ALL_EXEC  00001
#define GRP_READ  00040
#define GRP_WRITE 00020
#define GRP_EXEC  00010
#define USR_READ  00400
#define USR_WRITE 00200
#define USR_EXEC  00100

#define SUID      01000
#define SGID      02000


struct fs_node {
    int filetype;
    char filename[256];

    int permission;
    int uid;
    int gid;

    size_t (*len)(struct fs_node *n);
    void *(*buf)(struct fs_node *n);

    size_t (*read)(struct fs_node *n, void *data, size_t len);
    size_t (*write)(struct fs_node *n, const void *data, size_t len);

    // TO BE REMOVED
    struct ringbuf buffer;
    bool nonblocking;
    // />

    union {
        void *extra_data;
        uintptr_t extra_handle; // for fds with extra data in a vector
    };
};

struct pty_extra {
    struct ringbuf ring;
};

extern struct vector *fs_node_table;

void vfs_init();
void mount(struct fs_node *n, char *path);

struct syscall_ret sys_read(int fd, void *data, size_t len);
struct syscall_ret sys_write(int fd, const void *data, size_t len);
struct syscall_ret sys_dup2(int oldfd, int newfd);

#endif
