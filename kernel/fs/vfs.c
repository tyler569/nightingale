
#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <vector.h>
#include <syscall.h>
#include "vfs.h"

struct vector *fs_node_table;

ssize_t dev_zero_read(void *data_, size_t len) {
    char *data = data_;

    for (size_t i=0; i<len; i++) {
        data[i] = 0;
    }
    return len;
}

ssize_t dev_null_write(const void *data, size_t len) {
    return len;
}

ssize_t dev_inc_read(void *data_, size_t len) {
    char *data = data_;

    for (size_t i=0; i<len; i++) {
        data[i] = i;
    }
    return len;
}

ssize_t stdout_write(const void *data_, size_t len) {
    char *data = data_;

    for (size_t i=0; i<len; i++) {
        printf("%c", data[i]);
    }
    return len;
}

struct syscall_ret sys_read(int fd, void *data, size_t len) {
    
    // TEMP: fd's are global indecies into the fs_node_table.
    
    struct syscall_ret ret;

    if (fd > fs_node_table->len) {
        ret.error = 2; // TODO: make a real error for this
        return ret;
    }

    struct fs_node *node = vec_get(fs_node_table, fd);

    if (!node->read) {
        ret.error = 3; // TODO
        return ret;
    }

    node->read(data, len);

    ret.error = 0;
    ret.value = len;
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

    node->write(data, len);

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
}

