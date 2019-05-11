
#include <ng/basic.h>
#include <ng/fs.h>
#include <ng/malloc.h>
#include <ng/print.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <ds/dmgr.h>
#include <ds/ringbuf.h>
#include <ds/vector.h>
#include <fs/tarfs.h>
#include <stddef.h>
#include <stdint.h>
#include "char_devices.h"
#include "membuf.h"

struct dmgr fs_node_table = {0};

extern struct tar_header *initfs;

static struct fs_node *fs_node_region = NULL;
static struct list fs_node_free_list = {.head = NULL, .tail = NULL};

struct fs_node *new_file_slot() {
        if (fs_node_free_list.head) {
                return list_pop_front(&fs_node_free_list);
        } else {
                return fs_node_region++;
        }
}

void free_file_slot(struct fs_node *defunct) {
        list_prepend(&fs_node_free_list, defunct);
}

// should these not be staic?
struct fs_node *dev_zero = &(struct fs_node){0};
struct fs_node *dev_serial = &(struct fs_node){0};
struct open_fd *dev_stdin = &(struct open_fd){0};
struct open_fd *dev_stdout = &(struct open_fd){0};
struct open_fd *dev_stderr = &(struct open_fd){0};

#define FS_NODE_BOILER(fd, perm) \
        struct open_fd *ofd = dmgr_get(&running_process->fds, fd); \
        if (ofd == NULL) { RETURN_ERROR(EBADF); } \
        if (!(ofd->flags & perm)) { RETURN_ERROR(EPERM); } \
        struct fs_node *node = ofd->node;

struct syscall_ret sys_open(const char *filename, int flags) {
        if (flags != O_RDONLY) {
                RETURN_ERROR(ETODO);
        }

        void *file = tarfs_get_file(initfs, filename);
        if (!file) {
                RETURN_ERROR(ENOENT);
        }

        struct fs_node *new_file = new_file_slot();
        new_file->filetype = MEMORY_BUFFER;
        new_file->permission = USR_READ;
        new_file->len = tarfs_get_len(initfs, filename);
        new_file->ops.read = membuf_read;
        new_file->ops.seek = membuf_seek;
        new_file->extra.memory = file;
        if (new_file->ops.open) {
                new_file->ops.open(new_file);
        }

        struct open_fd *new_open_fd = malloc(sizeof(struct open_fd));
        new_open_fd->node = new_file;
        new_file->refcnt++;
        new_open_fd->flags = USR_READ;
        new_open_fd->off = 0;

        size_t new_fd = dmgr_insert(&running_process->fds, new_file);

        RETURN_VALUE(new_fd);
}

struct syscall_ret sys_read(int fd, void *data, size_t len) {
        FS_NODE_BOILER(fd, USR_READ);

        ssize_t value;
        while ((value = node->ops.read(ofd, data, len)) == -1) {
                if (node->flags & FILE_NONBLOCKING)
                        RETURN_ERROR(EWOULDBLOCK);

                block_thread(&node->blocked_threads);
        }
        RETURN_VALUE(value);
}

struct syscall_ret sys_write(int fd, const void *data, size_t len) {
        FS_NODE_BOILER(fd, USR_WRITE);

        len = node->ops.write(ofd, data, len);
        RETURN_VALUE(len);
}

struct syscall_ret sys_dup2(int oldfd, int newfd) {
        struct open_fd *ofd = dmgr_get(&running_process->fds, oldfd);
        RETURN_ERROR(ETODO);

        RETURN_VALUE(newfd);
}

struct syscall_ret sys_seek(int fd, off_t offset, int whence) {
        if (whence > SEEK_END || whence < SEEK_SET) {
                RETURN_ERROR(EINVAL);
        }

        struct open_fd *ofd = dmgr_get(&running_process->fds, fd);
        if (ofd == NULL) {
                RETURN_ERROR(EBADF);
        }

        struct fs_node *node = ofd->node;
        if (!node->ops.seek) {
                RETURN_ERROR(EINVAL);
        }

        off_t old_off = ofd->off;

        node->ops.seek(ofd, offset, whence);

        if (ofd->off < 0) {
                ofd->off = old_off;
                RETURN_ERROR(EINVAL);
        }

        RETURN_VALUE(ofd->off);
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

                                struct open_fd *ofd = dmgr_get(&running_process->fds, fds[i].fd);
                                if (ofd == NULL) {
                                        RETURN_ERROR(EBADF);
                                }
                                struct fs_node *node = ofd->node;

                                if (!node) {
                                        RETURN_ERROR(EBADF);
                                }

                                if (node->filetype != PTY) {
                                        RETURN_ERROR(ETODO);
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

void make_tar_file(const char *name, struct fs_node *dir) {}

void make_dir(const char *name, struct fs_node *dir) {}

struct fs_node *root_node = &(struct fs_node) {
        .filetype = DIRECTORY,
        .filename = "",
        .permission = USR_READ | USR_WRITE,
        .uid = 0,
        .gid = 0,
}

void vfs_init() {
        fs_node_region = vmm_reserve(20 * 1024);

        // make all the tarfs files into fs_nodes and put into directories

        dev_zero->ops.read = dev_zero_read;

        dev_serial->ops.write = serial_write;
        dev_serial->ops.read = file_buf_read;
        dev_serial->filetype = PTY;
        emplace_ring(&dev_serial->extra.ring, 128);
}

