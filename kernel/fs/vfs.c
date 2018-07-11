
#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <print.h>
#include <malloc.h>
#include <vector.h>
#include <syscall.h>
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

void mkdir(struct fs_node *parent, char *name) {
    // TODO (below is getting crazy)
}

void init_vfs() {
    struct fs_node temp;

    fs_node_table = malloc(sizeof(struct vector));
    vec_init(fs_node_table, struct fs_node);

    temp.type = VFS_TYPE_DIRECTORY,
    temp.name[0] = 0,
    temp.permission = 0755,
    temp.uid = 0,
    temp.gid = 0,
    temp.read = NULL,
    temp.write = NULL,
    temp.parent_directory = NULL,
    temp.child_nodes = NULL, // for now
    temp.extra_data = NULL,

    vec_push(fs_node_table, &temp);
    struct fs_node *fs_root = vec_get(fs_node_table, 0);
    vec_init(&fs_root->child_nodes, size_t);

    /* '/dev' */

    temp.name = "dev";
    new_ix = vec_push(fs_node_table, &temp);

    // fs_root is invalidated by push, get it back
    struct fs_node *fs_root = vec_get(fs_node_table, 0);

    // save the index into the nodes table we made for /dev into /'s children
    size_t dev_zero_ix = vec_push(&fs_root->child_nodes, &new_ix);

    temp.type = VFS_TYPE_CHAR_DEV,
    temp.name = "zero",
    temp.read = dev_zero_read,
    temp.write = NULL,

    new_ix = vec_push(fs_node_table, &temp);
    vec_push(vec_get(fs_node_table, 


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

