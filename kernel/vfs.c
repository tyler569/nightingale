
#include <basic.h>
#include <ng/fs.h>
#include <ng/malloc.h>
#include <ng/print.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <ng/dmgr.h>
#include <ng/ringbuf.h>
#include <ng/vector.h>
#include <ng/tarfs.h>
#include <nc/errno.h>
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
struct fs_node *fs_root_node = &(struct fs_node) {
        .filetype = DIRECTORY,
        .filename = "",
        .permission = USR_READ | USR_WRITE,
        .uid = 0,
        .gid = 0,
};

struct fs_node _v_dev_zero = {0};
struct fs_node _v_dev_serial = {0};
struct fs_node _v_dev_serial2 = {0};

struct fs_node *dev_zero = &_v_dev_zero;
struct fs_node *dev_serial = &_v_dev_serial;
struct fs_node *dev_serial2 = &_v_dev_serial2;

struct open_fd _v_ofd_stdin = {
        .flags = USR_READ,
};
struct open_fd _v_ofd_stdout = {
        .flags = USR_WRITE,
};
struct open_fd _v_ofd_stderr = {
        .flags = USR_WRITE,
};

struct open_fd *ofd_stdin = &_v_ofd_stdin;
struct open_fd *ofd_stdout = &_v_ofd_stdout;
struct open_fd *ofd_stderr = &_v_ofd_stderr;

#define FS_NODE_BOILER(fd, perm) \
        struct open_fd *ofd = dmgr_get(&running_process->fds, fd); \
        if (ofd == NULL) { return -EBADF; } \
        if ((ofd->flags & perm) != perm) { return -EPERM; } \
        struct fs_node *node = ofd->node;

struct fs_node *find_fs_node_child(struct fs_node *node, const char *filename) {
        struct list_n *chld_list = node->children.head;
        struct fs_node *child;

        // printf("trying to find '%s' in '%s'\n", filename, node->filename);

        for (; chld_list; chld_list = chld_list->next) {
                child = chld_list->v;
                if (strcmp(child->filename, filename) == 0) {
                        return child;
                }
        }

        return NULL;
}

/*
struct fs_node *get_file_by_name(struct fs_node *root, char *filename) {
        struct fs_node *node = root;

        char name_buf[256];

        while (filename && node) {
                filename = str_until(filename, name_buf, "/");

                if (strlen(name_buf) == 0) {
                        continue;
                }

                node = find_fs_node_child(node, name_buf);
        }
        
        return node;
}
*/

struct fs_node *fs_resolve_relative_path(struct fs_node *root, const char *filename) {
        struct fs_node *node = root;

        if (!node || filename[0] == '/') {
                node = fs_root_node;
        }

        char name_buf[MAX_FILENAME];

        while (filename && node) {
                filename = str_until(filename, name_buf, "/");

                if (strlen(name_buf) == 0) {
                        continue;
                } else if (strcmp(name_buf, ".") == 0) {
                        continue;
                } else if (strcmp(name_buf, "..") == 0) {
                        node = node->parent;
                } else {
                        node = find_fs_node_child(node, name_buf);
                }
        }
        
        return node;
}

struct fs_node *create_file(struct fs_node *root, char *filename, int flags) {
        if (root->filetype != DIRECTORY)  return (void *)-EACCES;

        if (strchr(filename, '/') != NULL) {
                printf("TODO: support creating files in dirs\n");
                return (void *)-ETODO;
        }

        struct fs_node *node = zmalloc(sizeof(struct fs_node));
        node->filetype = MEMORY_BUFFER;
        strcpy(node->filename, filename);
        node->refcnt = 0; // gets incremented in do_sys_open
        node->flags = 0;
        node->permission = 0;
        node->permission = USR_READ | USR_WRITE; // TODO: umask
        node->len = 0;
        node->ops.read = membuf_read;
        node->ops.write = membuf_write;
        node->ops.seek = membuf_seek;

        node->memory = zmalloc(1024);
        node->capacity = 1024;

        node->parent = root;
        list_append(&node->parent->children, node);

        return node;
}

sysret do_sys_open(struct fs_node *root, char *filename, int flags) {
        struct fs_node *node = fs_resolve_relative_path(root, filename);

        if (!node) {
                if (flags & O_CREAT) {
                        node = create_file(root, filename, flags);
                        if is_error(node)  return (intptr_t)node;
                } else {
                        return -ENOENT;
                }
        }

        if ((flags & O_RDONLY) && !(node->permission & USR_READ)) {
                return -EPERM;
        }

        if ((flags & O_WRONLY) && !(node->permission & USR_WRITE)) {
                return -EPERM;
        }

        if ((flags & O_WRONLY && flags & O_TRUNC)) {
                // is that it?
                node->len = 0;
        }

        struct open_fd *new_open_fd = zmalloc(sizeof(struct open_fd));
        new_open_fd->node = node;
        node->refcnt++;
        if (flags & O_RDONLY)
                new_open_fd->flags |= USR_READ;
        if (flags & O_WRONLY)
                new_open_fd->flags |= USR_WRITE;
        new_open_fd->off = 0;

        if (node->ops.open)  node->ops.open(new_open_fd);

        size_t new_fd = dmgr_insert(&running_process->fds, new_open_fd);

        return new_fd;
}

sysret sys_open(char *filename, int flags) {
        return do_sys_open(running_thread->cwd, filename, flags);
}

sysret sys_close(int fd) {
        FS_NODE_BOILER(fd, 0);
        if (node->ops.close)  node->ops.close(ofd);
        dmgr_drop(&running_process->fds, fd);
        free(ofd);
        return 0;
}

sysret sys_openat(int fd, char *filename, int flags) {
        FS_NODE_BOILER(fd, 0);
        if (node->filetype != DIRECTORY)  return -EBADF;

        return do_sys_open(node, filename, flags);
}

sysret sys_read(int fd, void *data, size_t len) {
        FS_NODE_BOILER(fd, USR_READ);

        ssize_t value;
        while ((value = node->ops.read(ofd, data, len)) == -1) {
                if (node->flags & FILE_NONBLOCKING)
                        return -EWOULDBLOCK;

                if (node->signal_eof) {
                        node->signal_eof = 0;
                        return 0;
                }

                block_thread(&node->blocked_threads);
        }
        return value;
}

sysret sys_write(int fd, const void *data, size_t len) {
        FS_NODE_BOILER(fd, USR_WRITE);

        len = node->ops.write(ofd, data, len);
        return len;
}

sysret sys_dup2(int oldfd, int newfd) {
        struct open_fd *ofd = dmgr_get(&running_process->fds, oldfd);
        if (!ofd)  return -EBADF;

        struct open_fd *nfd = dmgr_get(&running_process->fds, newfd);
        if (!nfd)  return -ETODO;

        // if newfd is extant, dup2 closes it silently.
        if (nfd->node->ops.close)
                nfd->node->ops.close(nfd);

        // free(nfd); <- probematic? should it be?

        dmgr_set(&running_process->fds, newfd, ofd);

        return newfd;
}

sysret sys_seek(int fd, off_t offset, int whence) {
        if (whence > SEEK_END || whence < SEEK_SET) {
                return -EINVAL;
        }

        struct open_fd *ofd = dmgr_get(&running_process->fds, fd);
        if (ofd == NULL) {
                return -EBADF;
        }

        struct fs_node *node = ofd->node;
        if (!node->ops.seek) {
                return -EINVAL;
        }

        off_t old_off = ofd->off;

        node->ops.seek(ofd, offset, whence);

        if (ofd->off < 0) {
                ofd->off = old_off;
                return -EINVAL;
        }

        return ofd->off;
}

sysret sys_poll(struct pollfd *fds, nfds_t nfds, int timeout) {
        if (nfds < 0) {
                return -EINVAL;
        } else if (nfds == 0) {
                return 0;
        }

        if (timeout > 0) {
                return -ETODO;
        }

        for (int i = 0; i < nfds; i++) {
                if (fds[i].fd < 0) {
                        continue;
                }

                struct open_fd *ofd = dmgr_get(&running_process->fds, fds[i].fd);
                if (ofd == NULL) {
                        return -EBADF;
                }
                struct fs_node *node = ofd->node;

                if (!node) {
                        return -EBADF;
                }

                if (node->filetype != TTY) {
                        // This is still terrible
                        return -ETODO;
                }

                if (node->ring.len != 0) {
                        fds[i].revents = POLLIN;
                        return 1;
                }
        }

        return 0;
}

struct fs_node *make_dir(const char *name, struct fs_node *dir) {
        struct fs_node *new_dir = zmalloc(sizeof(struct fs_node));
        new_dir->filetype = DIRECTORY;
        strcpy(new_dir->filename, name);
        new_dir->permission = USR_READ | USR_WRITE;
        new_dir->parent = dir;

        return new_dir;
}

void put_file_in_dir(struct fs_node *file, struct fs_node *dir) {
        file->parent = dir;
        list_append(&dir->children, file);
}

extern struct tar_header *initfs;

struct fs_node *make_tar_file(const char *name, size_t len, void *file) {
        struct fs_node *node = new_file_slot();
        strcpy(node->filename, name);
        node->filetype = MEMORY_BUFFER;
        node->permission = USR_READ | USR_EXEC;
        node->len = len;
        node->ops.read = membuf_read;
        node->ops.seek = membuf_seek;
        node->memory = file;
        node->capacity = -1;
        return node;
}

void vfs_init() {
        fs_root_node->parent = fs_root_node;

        struct fs_node *dev = make_dir("dev", fs_root_node);
        struct fs_node *bin = make_dir("bin", fs_root_node);
        put_file_in_dir(dev, fs_root_node);
        put_file_in_dir(bin, fs_root_node);

        fs_node_region = vmm_reserve(20 * 1024);

        // make all the tarfs files into fs_nodes and put into directories

        dev_zero->ops.read = dev_zero_read;
        dev_zero->permission = USR_READ;
        strcpy(dev_zero->filename, "zero");

        put_file_in_dir(dev_zero, dev);

        ofd_stdin->node = dev_serial;
        ofd_stdout->node = dev_serial;
        ofd_stderr->node = dev_serial;

        dev_serial->ops.write = dev_serial_write;
        dev_serial->ops.read = dev_serial_read;
        dev_serial->filetype = TTY;
        dev_serial->permission = USR_READ | USR_WRITE;
        emplace_ring(&dev_serial->ring, 128);
        dev_serial->tty = &serial_tty;
        strcpy(dev_serial->filename, "serial");

        put_file_in_dir(dev_serial, dev);

        dev_serial2->ops.write = dev_serial_write;
        dev_serial2->ops.read = dev_serial_read;
        dev_serial2->filetype = TTY;
        dev_serial2->permission = USR_READ | USR_WRITE;
        emplace_ring(&dev_serial2->ring, 128);
        dev_serial2->tty = &serial_tty2;
        strcpy(dev_serial2->filename, "serial2");

        put_file_in_dir(dev_serial2, dev);

        struct tar_header *tar = initfs;
        vmm_map_range((uintptr_t)tar, (uintptr_t)tar - VMM_VIRTUAL_OFFSET,
                      3000000, PAGE_PRESENT);
        while (tar->filename[0]) {
                size_t len = tar_convert_number(tar->size);

                char *filename = tar->filename;
                void *file_content = ((char *)tar) + 512;
                struct fs_node *new_node =
                        make_tar_file(filename, len, file_content);
                put_file_in_dir(new_node, bin);

                uintptr_t next_tar = (uintptr_t)tar;
                next_tar += len + 0x200;
                next_tar = round_up(next_tar, 512);

                tar = (void *)next_tar;
        }
}

void vfs_print_tree(struct fs_node *root, int indent) {
        for (int i=0; i<indent; i++) {
                printf("  ");
        }
        printf("+ '%s' ", root->filename);
        printf(root->permission & USR_READ ? "r" : "-");
        printf(root->permission & USR_WRITE ? "w" : "-");
        printf(root->permission & USR_EXEC ? "x" : "-");
        printf("\n");

        if (root->filetype == DIRECTORY) {
                struct list_n *ch = root->children.head;
                if (!ch)
                        printf("CH IS NULL\n");
                for (; ch; ch = ch->next) {
                        vfs_print_tree(ch->v, indent + 1);
                }
        }
}

