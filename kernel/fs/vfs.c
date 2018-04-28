
#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <print.h>
#include <vector.h>
#include <syscall.h>
#include <ringbuf.h>
#include "vfs.h"

struct vector *fs_node_table;

ssize_t dev_zero_read(struct fs_node *n, void *data_, size_t len) {
    char *data = data_;

    for (size_t i=0; i<len; i++) {
        data[i] = 0;
    }
    return len;
}

ssize_t dev_null_write(struct fs_node *n, const void *data, size_t len) {
    return len;
}

ssize_t dev_inc_read(struct fs_node *n, void *data_, size_t len) {
    char *data = data_;

    for (size_t i=0; i<len; i++) {
        data[i] = i;
    }
    return len;
}

ssize_t stdout_write(struct fs_node *n, const void *data_, size_t len) {
    const char *data = data_;

    for (size_t i=0; i<len; i++) {
        printf("%c", data[i]);
    }
    return len;
}

ssize_t file_buf_read(struct fs_node *n, void *data_, size_t len) {
    char *data = data_;

    size_t count = ring_read(&n->buffer, data, len);

    if (count == 0) {
        return -1;
    }

    return count;
}

struct syscall_ret sys_read(int fd, void *data, size_t len) {
    
    // TEMP: fd's are global indecies into the fs_node_table.
    
    struct syscall_ret ret;

    if (fd > fs_node_table->len) {
        ret.error = -3; // TODO: make a real error for this
        return ret;
    }

    struct fs_node *node = vec_get(fs_node_table, fd);

    if (!node->read) {
        ret.error = -4; // TODO make a real error for this - perms?
        return ret;
    }

    if (node->nonblocking) {
        if ((ret.value = node->read(node, data, len)) == -1) {
            ret.error = EWOULDBLOCK;
            ret.value = 0;
        } else {
            ret.error = SUCCESS;
        }
    } else {
        while ((ret.value = node->read(node, data, len)) == -1) {
            asm volatile ("hlt");
        }
        ret.error = SUCCESS;
    }

    return ret;
}

struct syscall_ret sys_write(int fd, const void *data, size_t len) {
    
    // TEMP: fd's are global indecies into the fs_node_table.
    
    struct syscall_ret ret;

    if (fd > fs_node_table->len) {
        ret.error = 2; // TODO: make a real error for this
        return ret;
    }

    struct fs_node *node = vec_get(fs_node_table, fd);

    if (!node->write) {
        ret.error = 3; // TODO
        return ret;
    }

    node->write(node, data, len);

    ret.error = 0;
    ret.value = len;
    return ret;
}

void init_vfs() {
    fs_node_table = new_vec(struct fs_node);

    struct fs_node dev_zero = { .read = dev_zero_read };
    vec_push(fs_node_table, &dev_zero);

    struct fs_node dev_stdout = { .write = stdout_write };
    vec_push(fs_node_table, &dev_stdout);

    struct fs_node dev_null = { .write = dev_null_write };
    vec_push(fs_node_table, &dev_null);
    
    struct fs_node dev_inc = { .read = dev_inc_read };
    vec_push(fs_node_table, &dev_inc);

    struct fs_node dev_serial = { .read = file_buf_read, .nonblocking = false };
    emplace_ring(&dev_serial.buffer, 128);
    vec_push(fs_node_table, &dev_serial);

}

