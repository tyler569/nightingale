
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
#include "vfs.h"

struct vector *fs_node_table;

struct syscall_ret sys_open(const char *filename, int flags) {
    struct syscall_ret ret = { 0, 0 };
    return ret;
}

struct syscall_ret sys_read(int fd, void *data, size_t len) {
    // TEMP: fd's are global indecies into the fs_node_table.
    struct syscall_ret ret;
    // int call_unique = count_reads++;
    // printf("ENTERING READ: read(%i):%i\n", fd, call_unique);
    struct vector* fds = &running_process->fds;
    if (fd > fds->len) {
        ret.error = -3; // TODO: make a real error for this
        return ret;
    }
    // dispatch into the fs_node_table based on process fd mappings
    // first we get the fs handle from the fd table on the process
    // and then index that handle into the real node table.
    //
    // something something open later.
    size_t file_handle = vec_get_value(fds, fd);
    struct fs_node* node = vec_get(fs_node_table, file_handle);
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
        while (
            // printf("read(%i):%i calling %lx\n", fd, call_unique, node->read), // REMOVE DEBUG
            (ret.value = node->read(node, data, len)) == -1) {
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
    size_t file_handle = vec_get_value(&running_process->fds, fd);
    struct fs_node *node = vec_get(fs_node_table, file_handle);
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
    fs_node_table = malloc(sizeof(*fs_node_table));
    vec_init(fs_node_table, struct fs_node);

    struct fs_node dev_zero = { .read = dev_zero_read };
    vec_push(fs_node_table, &dev_zero);

    struct fs_node dev_stdout = { .write = stdout_write };
    vec_push(fs_node_table, &dev_stdout);

    struct fs_node dev_null = { .write = dev_null_write };
    vec_push(fs_node_table, &dev_null);
    
    struct fs_node dev_inc = { .read = dev_inc_read };
    vec_push(fs_node_table, &dev_inc);

    // TODO: add serial writing
    struct fs_node dev_serial = { .read = file_buf_read, .nonblocking = false };
    emplace_ring(&dev_serial.buffer, 128);
    vec_push(fs_node_table, &dev_serial);
}

