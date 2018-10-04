
#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <print.h>
#include <malloc.h>
#include <vector.h>
#include <syscall.h>
#include <thread.h>
#include <ringbuf.h>
#include "char_devices.h"
#include "tarfs.h"
#include "membuf.h"
#include "vfs.h"

struct vector *fs_node_table;

extern struct tar_header* initfs;

struct syscall_ret sys_open(const char* filename, int flags) {
    if (flags) {
        // TODO
        RETURN_ERROR(EINVAL);
    }

    void* file = tarfs_get_file(initfs, filename);
    
    struct fs_node new_file = {
        .filetype = MEMORY_BUFFER,
        .permission = USR_READ,
        .len = tarfs_get_len(initfs, filename),
        .read = membuf_read,
        .seek = membuf_seek,
        .extra_data = file,
    };

    size_t new_file_id = vec_push(fs_node_table, &new_file);
    size_t new_fd = vec_push_value(&running_process->fds, new_file_id);

    RETURN_VALUE(new_fd);
}

struct syscall_ret sys_read(int fd, void* data, size_t len) {
    struct vector* fds = &running_process->fds;
    if (fd > fds->len) {
        RETURN_ERROR(EBADF);
    }

    size_t file_handle = vec_get_value(fds, fd);
    struct fs_node* node = vec_get(fs_node_table, file_handle);
    if (!node->read) {
        RETURN_ERROR(EPERM);
    }

    ssize_t value;
    while ((value = node->read(node, data, len)) == -1) {
        if (node->nonblocking)  RETURN_ERROR(EWOULDBLOCK);

        block_thread(&node->blocked_threads);
    }
    RETURN_VALUE(value);
}

struct syscall_ret sys_write(int fd, const void *data, size_t len) {
    if (fd > fs_node_table->len) {
        RETURN_ERROR(EBADF);
    }
    size_t file_handle = vec_get_value(&running_process->fds, fd);
    struct fs_node *node = vec_get(fs_node_table, file_handle);
    if (!node->write) {
        RETURN_ERROR(EPERM);
    }
    len = node->write(node, data, len);
    RETURN_VALUE(len);
}

struct syscall_ret sys_dup2(int oldfd, int newfd) {
    if (oldfd > running_process->fds.len) {
        RETURN_ERROR(EBADF);
    }
    size_t file_handle = vec_get_value(&running_process->fds, oldfd);
    vec_set_value_ex(&running_process->fds, newfd, file_handle);
    RETURN_VALUE(newfd);
}

struct syscall_ret sys_seek(int fd, off_t offset, int whence) {
    if (whence > SEEK_END || whence < SEEK_SET) {
        RETURN_ERROR(EINVAL);
    }

    if (fd > fs_node_table->len) {
        RETURN_ERROR(EBADF);
    }
    size_t file_handle = vec_get_value(&running_process->fds, fd);
    struct fs_node *node = vec_get(fs_node_table, file_handle);

    if (!node->seek) {
        RETURN_ERROR(EINVAL);
    }

    off_t old_off = node->off;

    node->seek(node, offset, whence);

    if (node->off < 0) {
        node->off = old_off;
        RETURN_ERROR(EINVAL);
    }

    RETURN_VALUE(node->off);
}

void vfs_init() {
    fs_node_table = malloc(sizeof(*fs_node_table));
    vec_init(fs_node_table, struct fs_node);

    struct fs_node dev_zero = { .read = dev_zero_read };
    vec_push(fs_node_table, &dev_zero);

    struct fs_node dev_serial = {
        .write = serial_write,
        .read = file_buf_read,
        .nonblocking = false
    };
    emplace_ring(&dev_serial.buffer, 128);
    vec_push(fs_node_table, &dev_serial);
    
    // TODO: There's currently no way to use these, they need
    // to be supported by open()
    struct fs_node dev_null = { .write = dev_null_write };
    vec_push(fs_node_table, &dev_null);
    
    struct fs_node dev_inc = { .read = dev_inc_read };
    vec_push(fs_node_table, &dev_inc);
}

