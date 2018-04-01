
#ifndef NIGHTINGALE_FS_VFS_H
#define NIGHTINGALE_FS_VFS_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <vector.h>
#include <syscall.h>

struct fs_node {
    ssize_t (*read)(void *data, size_t len);
    ssize_t (*write)(const void *data, size_t len);
};

extern struct vector *fs_node_table;

void init_vfs();

struct syscall_ret sys_read(int fd, void *data, size_t len);
struct syscall_ret sys_write(int fd, const void *data, size_t len);

#endif
