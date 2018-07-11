
#ifndef NIGHTINGALE_FS_VFS_H
#define NIGHTINGALE_FS_VFS_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <vector.h>
// #include <ptr_vector.h>
#include <syscall.h>
#include <ringbuf.h>

enum filetype {
    CHAR_DEV,           // like /dev/null
    PTY,                // like /dev/serial
    MEMORY_BUFFER,      // like initfs
    ON_DISK,

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

enum vfs_filetype {
    VFS_TYPE_INVALID = 0,
    VFS_TYPE_DIRECTORY,
    VFS_TYPE_CHAR_DEV,
    VFS_TYPE_FILE,
};

// See below
// typedef size_t (read_fn)(struct fs_node *n, void *data, size_t len);
// typedef size_t (write_fn)(struct fs_node *n, void *data, size_t len);

// Thought: do these functions return an error union?
// if the syscalls do then theoretically read and write have to..
// can I abbreviate that and not break my covenants?
// struct syscall_ret is a lot of letters;
//
// typedef struct foobar {
//     size_t v;
//     size_t err;
// } maybe;
//
// maybe x = {};
// x.v = 10;
// 
// I don't think I like that.
// This does warrant more consideration

struct fs_node {
    int type;
    char name[128]; // does not include any directories

    int permission;
    int uid;
    int gid;

    // size_t (*len)(struct fs_node *n);
    // void *(*buf)(struct fs_node *n);

    size_t (*read)(struct fs_node *n, void *data, size_t len);
    size_t (*write)(struct fs_node *n, const void *data, size_t len);

    // Is this lying?
    // read_fn *read;
    // write_fn *write;

    struct fs_node_t *parent_directory;
    struct vector child_nodes;

    // < TO BE REMOVED
    struct ringbuf buffer;
    bool nonblocking;
    // />

    // ?
    void *extra_data;
};

struct pty_extra {
    struct ringbuf ring;
};

// if root is node 0, we can't maintain a pointer to it because the inside
// of a vector is ephemoral and there's no guarantee it would stick around
extern struct vector *fs_node_table;

void init_vfs();
void mount(struct fs_node *n, char *path);

struct syscall_ret sys_read(int fd, void *data, size_t len);
struct syscall_ret sys_write(int fd, const void *data, size_t len);

#endif

