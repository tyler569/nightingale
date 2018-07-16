
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
    // am assuming O_RDWR
    // no creation is supported.

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

size_t make_fs_node(struct fs_node *parent, char *name, int type, void *read, void *write) {
    struct fs_node temp = {0};
    temp.type = type;
    strcpy(temp.name, name);
    temp.permission = 0755;
    temp.uid = 0;
    temp.gid = 0;
    temp.read = read;
    temp.write = write;
    temp.parent_directory = parent;
    temp.extra_data = NULL;

    if (type == VFS_TYPE_DIRECTORY) {
        vec_init(&temp.child_nodes, struct fs_node);
    }

    if (parent) {
        print_vector(&parent->child_nodes);
        return vec_push(&parent->child_nodes, &temp);
    } else {
        printf("Created a node with no parent, this had better be root!\n");
        return 0;
    }
}

struct fs_node root;

void init_vfs() {
    make_fs_node(NULL, "", VFS_TYPE_DIRECTORY, NULL, NULL);

    int dev_ix = make_fs_node(&root, "dev", VFS_TYPE_DIRECTORY, NULL, NULL);

    make_fs_node(vec_get(&root.child_nodes, dev_ix), "zero", VFS_TYPE_CHAR_DEV, dev_zero_read, NULL);
    make_fs_node(vec_get(&root.child_nodes, dev_ix), "stdout", VFS_TYPE_CHAR_DEV, NULL, stdout_write);
    make_fs_node(vec_get(&root.child_nodes, dev_ix), "null", VFS_TYPE_CHAR_DEV, NULL, dev_null_write);
    make_fs_node(vec_get(&root.child_nodes, dev_ix), "inc", VFS_TYPE_CHAR_DEV, dev_inc_read, NULL);

    int serial_ix = make_fs_node(vec_get(&root.child_nodes, dev_ix), "serial", VFS_TYPE_CHAR_DEV, file_buf_read, NULL);

    struct fs_node *dev = vec_get(&root.child_nodes, dev_ix);
    struct fs_node *serial = vec_get(&dev->child_nodes, serial_ix);
    emplace_ring(&serial->buffer, 128);
}

struct fs_node *vfs_find_child(struct fs_node *parent, char *name) {
    if (parent->type != VFS_TYPE_DIRECTORY) {
        return NULL;
    }
    struct fs_node *try;
    for (size_t i=0; i<parent->child_nodes.len; i+=1) {
        try = vec_get(&parent->child_nodes, i);
        if (strcmp(try->name, name) == 0) {
            return try;
        }
    }
    return NULL;
}

char *strcpy_to(char *dest, char *src, char delim) {
    char *s = strchr(src, delim);
    if (s) {  
        strncpy(dest, src, s-src);
        dest[s-src] = 0;
        return s;
    } else {
        strcpy(dest, src);
        return NULL;
    }
}

struct fs_node *vfs_get_file_by_path(char *filename) {
    char *path = filename;
    if (filename[0] != '/') {
        return NULL;
    }
    char sub_path[128];
    struct fs_node *step = &root;
    while ((filename = strcpy_to(sub_path, filename+1, '/'))) {
        step = vfs_find_child(step, sub_path);
        if (!step) {
            printf("Could not find file %s in path %s\n", sub_path, path);
            return NULL;
        }
    }
    return step;
}

