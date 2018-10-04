
#ifndef NIGHTINGALE_FS_VFS_H
#define NIGHTINGALE_FS_VFS_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <vector.h>
#include <syscall.h>
#include <ringbuf.h>
#include <queue.h>

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

typedef int64_t off_t;

struct fs_node {
    int filetype;
    char filename[256];

    int permission;
    int uid;
    int gid;

    // size_t (*len)(struct fs_node *n);
    // void* (*buf)(struct fs_node *n);

    /*
     * TODO: Do I have to store these here?
     * Can I have the function pointers be in a global table of all file
     * types that is indexed by the filetype?
     *
     * That requires primarily that most files are using a small set of
     * functions to implement read() and write(), which rules out
     * things like having a proc/-like filesystem where files are made
     * with custom functions backing read() and write()..  Maybe there's
     * a more elegant way to do that anyway, or perhaps a "other" file
     * type with one of the extra pointers pointing to custom
     * implementations of read and write?  That's a thought as well.
     */
    ssize_t (*read)(struct fs_node* n, void* data, size_t len);
    ssize_t (*write)(struct fs_node* n, const void* data, size_t len);
    off_t (*seek)(struct fs_node* n, off_t offset, int whence);

    // TO BE REMOVED
    struct ringbuf buffer;
    bool nonblocking;
    // />
    
    off_t len;
    off_t off;
    
    struct queue blocked_threads;

    union {
        void *extra_data;
        uintptr_t extra_handle; // for fds with extra data in a vector
    };
};

struct pty_extra {
    struct ringbuf ring;
};

enum {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END,
};

struct vector *fs_node_table;

void vfs_init();
void mount(struct fs_node *n, char *path);

struct syscall_ret sys_open(const char* filename, int flags);
struct syscall_ret sys_read(int fd, void *data, size_t len);
struct syscall_ret sys_write(int fd, const void *data, size_t len);
struct syscall_ret sys_dup2(int oldfd, int newfd);
struct syscall_ret sys_seek(int fs, off_t offset, int whence);

#endif

