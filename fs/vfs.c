
#include <ng/basic.h>
#include <ng/malloc.h>
#include <ng/print.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ds/dmgr.h>
#include <ds/ringbuf.h>
#include <ds/vector.h>
#include <fs/tarfs.h>
#include <ng/fs.h>
#include <stddef.h>
#include <stdint.h>
#include "char_devices.h"
#include "membuf.h"

struct dmgr fs_node_table = {0};

extern struct tar_header *initfs;

static struct fs_node *fs_node_region = NULL;
static struct list *fs_node_free_list = {0};

struct syscall_ret sys_open(const char *filename, int flags) {
        if (flags) {
                // TODO
                RETURN_ERROR(EINVAL);
        }

        void *file = tarfs_get_file(initfs, filename);

        struct fs_node *new_file = malloc(sizeof(struct fs_node));
        new_file->filetype = MEMORY_BUFFER;
        new_file->permission = USR_READ;
        new_file->len = tarfs_get_len(initfs, filename);
        new_file->ops.read = membuf_read;
        new_file->ops.seek = membuf_seek;
        new_file->extra.memory = file;

        size_t new_file_id = dmgr_insert(&fs_node_table, new_file);
        size_t new_fd = vec_push_value(&running_process->fds, new_file_id);

        RETURN_VALUE(new_fd);
}

struct syscall_ret sys_read(int fd, void *data, size_t len) {
        struct vector *fds = &running_process->fds;
        if (fd > fds->len) {
                RETURN_ERROR(EBADF);
        }

        size_t file_handle = vec_get_value(fds, fd);
        struct fs_node *node = dmgr_get(&fs_node_table, file_handle);
        if (!node->ops.read) {
                RETURN_ERROR(EPERM);
        }

        ssize_t value;
        while ((value = node->ops.read(node, data, len)) == -1) {
                if (node->flags & FILE_NONBLOCKING)
                        RETURN_ERROR(EWOULDBLOCK);

                block_thread(&node->blocked_threads);
        }
        RETURN_VALUE(value);
}

struct syscall_ret sys_write(int fd, const void *data, size_t len) {
        size_t file_handle = vec_get_value(&running_process->fds, fd);
        struct fs_node *node = dmgr_get(&fs_node_table, file_handle);
        if (!node) {
                RETURN_ERROR(EBADF);
        }
        if (!node->ops.write) {
                RETURN_ERROR(EPERM);
        }
        len = node->ops.write(node, data, len);
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

        size_t file_handle = vec_get_value(&running_process->fds, fd);
        struct fs_node *node = dmgr_get(&fs_node_table, file_handle);
        if (!node) {
                RETURN_ERROR(EBADF);
        }
        if (!node->ops.seek) {
                RETURN_ERROR(EINVAL);
        }

        off_t old_off = node->off;

        node->ops.seek(node, offset, whence);

        if (node->off < 0) {
                node->off = old_off;
                RETURN_ERROR(EINVAL);
        }

        RETURN_VALUE(node->off);
}

struct syscall_ret sys_poll(struct pollfd *fds, nfds_t nfds, int timeout) {
        if (nfds < 0) {
                RETURN_ERROR(EINVAL);
        } else if (nfds == 0) {
                RETURN_VALUE(0);
        }

        for (int t = 0; t < timeout; t++) {
                for (int slow_down = 0; slow_down < 5000; slow_down++) {
                        for (int i = 0; i < nfds; i++) {
                                if (fds[i].fd < 0) {
                                        continue;
                                }

                                size_t file_handle = vec_get_value(
                                    &running_process->fds, fds[i].fd);
                                struct fs_node *node =
                                    dmgr_get(&fs_node_table, file_handle);

                                if (!node) {
                                        RETURN_ERROR(EBADF);
                                }

                                if (node->filetype != PTY) {
                                        RETURN_ERROR(-9); // unsupported
                                }

                                if (node->extra.ring.len != 0) {
                                        fds[i].revents = POLLIN;
                                        RETURN_VALUE(1);
                                }
                        }
                }
        }

        RETURN_VALUE(0);
}

void vfs_init() {
        dmgr_init(&fs_node_table);

        struct fs_node *dev_zero = calloc(sizeof(struct fs_node), 1);
        dev_zero->ops.read = dev_zero_read;
        dmgr_insert(&fs_node_table, dev_zero);

        struct fs_node *dev_serial = calloc(sizeof(struct fs_node), 1);
        dev_serial->ops.write = serial_write;
        dev_serial->ops.read = file_buf_read;
        dev_serial->filetype = PTY;
        emplace_ring(&dev_serial->extra.ring, 128);
        dmgr_insert(&fs_node_table, dev_serial);
}
