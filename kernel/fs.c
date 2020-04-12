
#include <basic.h>
#include <ng/fs.h>
#include <ng/malloc.h>
#include <ng/print.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <ng/dmgr.h>
#include <ng/ringbuf.h>
#include <ng/tarfs.h>
#include <nc/assert.h>
#include <nc/errno.h>
#include <nc/dirent.h>
#include <nc/fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include "char_devices.h"
#include "membuf.h"

struct dmgr file_table = {0};

extern struct tar_header *initfs;

/*
static struct file *file_region = NULL;
static struct list file_free_list = {.head = NULL, .tail = NULL};
*/

struct file *new_file_slot() {
        // there are pointers in here, zmalloc it
        return zmalloc(sizeof(struct file));
}

void free_file_slot(struct file *defunct) {
        free(defunct);
}

// should these not be static?
struct file *fs_root_node = &(struct file) {
        .filetype = FT_DIRECTORY,
        .filename = "",
        .permissions = USR_READ | USR_WRITE,
        .uid = 0,
        .gid = 0,
};

struct file _v_dev_zero = {0};
struct file _v_dev_serial = {0};
struct file _v_dev_serial2 = {0};

struct file *dev_zero = &_v_dev_zero;
struct file *dev_serial = &_v_dev_serial;
struct file *dev_serial2 = &_v_dev_serial2;

#define FS_NODE_BOILER(fd, perm) \
        struct open_file *ofd = dmgr_get(&running_process->fds, fd); \
        if (ofd == NULL) { return -EBADF; } \
        if ((ofd->flags & perm) != perm) { return -EPERM; } \
        struct file *node = ofd->node;

struct file *find_file_child(struct file *node, const char *filename) {
        struct file *child;

        list_foreach(&node->children, child, directory_siblings) {
                if (strcmp(child->filename, filename) == 0) {
                        return child;
                }
        }

        return NULL;
}

struct file *fs_resolve_relative_path(struct file *root, const char *filename) {
        struct file *node = root;

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
                        node = find_file_child(node, name_buf);
                }
        }
        
        return node;
}

void free_memory(struct file *f) {
        free(f->memory);
}

struct file *create_file(struct file *root, char *filename, int mode) {
        if (root->filetype != FT_DIRECTORY)  return (void *)-EACCES;

        if (strchr(filename, '/') != NULL) {
                printf("TODO: support creating files in dirs\n");
                return (void *)-ETODO;
        }

        struct file *node = zmalloc(sizeof(struct file));
        node->filetype = FT_BUFFER;
        strcpy(node->filename, filename);
        node->refcnt = 0; // gets incremented in do_sys_open
        node->flags = 0;
        node->permissions = mode;
        node->len = 0;
        node->read = membuf_read;
        node->write = membuf_write;
        node->seek = membuf_seek;

        node->memory = zmalloc(1024);
        node->destroy = free_memory;
        node->capacity = 1024;

        node->parent = root;
        list_append(&node->parent->children, node, directory_siblings);

        return node;
}

void destroy_file(struct file *defunct) {
        list_remove(&defunct->directory_siblings);
        if (defunct->destroy)
                defunct->destroy(defunct);
        free(defunct);
}

sysret do_sys_open(struct file *root, char *filename, int flags, int mode) {
        struct file *node = fs_resolve_relative_path(root, filename);

        if (!node) {
                if (flags & O_CREAT) {
                        node = create_file(root, filename, mode);
                        mode = USR_READ | USR_WRITE;
                        if is_error(node)  return (intptr_t)node;
                } else {
                        return -ENOENT;
                }
        }

        if ((flags & O_RDONLY) && !(node->permissions & USR_READ)) {
                return -EPERM;
        }

        if ((flags & O_WRONLY) && !(node->permissions & USR_WRITE)) {
                return -EPERM;
        }

        if ((flags & O_WRONLY && flags & O_TRUNC)) {
                // is that it?
                node->len = 0;
        }

        if (!(flags & O_CREAT)) {
                // mode argument is ignored
                mode = 0;
                if (flags & O_RDONLY)  mode |= USR_READ;
                if (flags & O_WRONLY)  mode |= USR_WRITE;
        }

        struct open_file *new_open_file = zmalloc(sizeof(struct open_file));
        new_open_file->node = node;
        node->refcnt++;
        new_open_file->flags = mode;
        new_open_file->off = 0;

        if (node->open)  node->open(new_open_file);

        size_t new_fd = dmgr_insert(&running_process->fds, new_open_file);

        return new_fd;
}

sysret sys_open(char *filename, int flags, int mode) {
        return do_sys_open(running_thread->cwd, filename, flags, mode);
}

void do_close_open_file(struct open_file *ofd) {
        struct file *node = ofd->node;
        if (node == (struct file *)0x4646464646464646) {
                mregion *r = allocation_region(ofd);
                printf("OFD FREED! (freed at %s)\n", mregion_last_location(r));
                assert(0);
        }

        node->refcnt--;
        if (node->close)  node->close(ofd);

        free(ofd);
}

sysret sys_close(int fd) {
        FS_NODE_BOILER(fd, 0);
        dmgr_drop(&running_process->fds, fd);
        do_close_open_file(ofd);
        return 0;
}

sysret sys_openat(int fd, char *filename, int flags, int mode) {
        FS_NODE_BOILER(fd, 0);
        if (node->filetype != FT_DIRECTORY)  return -EBADF;

        return do_sys_open(node, filename, flags, mode);
}

sysret sys_read(int fd, void *data, size_t len) {
        FS_NODE_BOILER(fd, USR_READ);

        if (node->filetype == FT_DIRECTORY) {
                return -EISDIR;
        }

        ssize_t value;
        while ((value = node->read(ofd, data, len)) == -1) {
                if (node->flags & FILE_NONBLOCKING)
                        return -EWOULDBLOCK;

                if (node->signal_eof) {
                        node->signal_eof = 0;
                        return 0;
                }

                block_thread(&node->blocked_threads);
        }

        /*
        printf("\"");
        for (int i=0; i<value; i++) {
                printf("%hhx ", ((char *)data)[i]);
        }
        printf("\"\n");
        */

        return value;
}

sysret sys_write(int fd, const void *data, size_t len) {
        FS_NODE_BOILER(fd, USR_WRITE);

        len = node->write(ofd, data, len);
        return len;
}

sysret sys_dup2(int oldfd, int newfd) {
        struct open_file *ofd = dmgr_get(&running_process->fds, oldfd);
        if (!ofd)  return -EBADF;

        struct open_file *nfd = dmgr_get(&running_process->fds, newfd);

        // printf("dup2: %i (\"%s\") -> %i (closes \"%s\")\n",
        //                 oldfd, ofd->node->filename,
        //                 newfd, nfd ? nfd->node->filename : "X");

        if (nfd) {
                do_close_open_file(nfd);
        }
        nfd = malloc(sizeof(struct open_file));

        memcpy(nfd, ofd, sizeof(struct open_file));
        dmgr_set(&running_process->fds, newfd, nfd);

        ofd->node->refcnt += 1;

        return newfd;
}

sysret sys_seek(int fd, off_t offset, int whence) {
        if (whence > SEEK_END || whence < SEEK_SET) {
                return -EINVAL;
        }

        struct open_file *ofd = dmgr_get(&running_process->fds, fd);
        if (ofd == NULL) {
                return -EBADF;
        }

        struct file *node = ofd->node;
        off_t old_off = ofd->off;
        if (node->seek) {
                node->seek(ofd, offset, whence);
        } else {
                // File is not seekable
                return -ESPIPE;
        }

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

                struct open_file *ofd = dmgr_get(&running_process->fds, fds[i].fd);
                if (ofd == NULL) {
                        return -EBADF;
                }
                struct file *node = ofd->node;

                if (!node) {
                        return -EBADF;
                }

                if (node->filetype != FT_TTY) {
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

sysret sys_getdirents(int fd, struct ng_dirent *buf, ssize_t count) {
        if (count < 0)  return -EINVAL;
        FS_NODE_BOILER(fd, USR_READ); // exec?

        if (node->filetype != FT_DIRECTORY)  return -EBADF;

        list *cursor = list_head(&node->children);

        int skip = ofd->off;
        while (skip > 0) {
                cursor = cursor->next;
                skip -= 1;

                if (cursor == &node->children) return 0; // EOF
        }

        ssize_t i = 0;
        for (i=0; i<count; i++) {
                if (cursor == &node->children)  break;
                struct file *child_node = list_entry(struct file, directory_siblings, cursor);
                buf[i].type = child_node->filetype;
                buf[i].permissions = child_node->permissions;
                strcpy(buf[i].filename, child_node->filename);

                cursor = cursor->next;
        }
        ofd->off += i;

        return i;
}

struct file *make_dir(const char *name, struct file *dir) {
        struct file *new_dir = new_file_slot();
        list_init(&new_dir->children);

        new_dir->filetype = FT_DIRECTORY;
        strcpy(new_dir->filename, name);
        new_dir->permissions = USR_READ | USR_WRITE;
        new_dir->parent = dir;

        return new_dir;
}

void put_file_in_dir(struct file *file, struct file *dir) {
        file->parent = dir;
        list_append(&dir->children, file, directory_siblings);
}

extern struct tar_header *initfs;

struct file *make_tar_file(const char *name, size_t len, void *file) {
        struct file *node = new_file_slot();
        strcpy(node->filename, name);
        node->filetype = FT_BUFFER;
        node->permissions = USR_READ | USR_EXEC;
        node->len = len;
        node->read = membuf_read;
        node->seek = membuf_seek;
        node->memory = file;
        node->capacity = -1;
        list_init(&node->blocked_threads);
        return node;
}

void vfs_init(uintptr_t initfs_len) {
        list_init(&fs_root_node->children);
        fs_root_node->parent = fs_root_node;
        strcpy(fs_root_node->filename, "<root>");

        struct file *dev = make_dir("dev", fs_root_node);
        struct file *bin = make_dir("bin", fs_root_node);
        struct file *proc = make_dir("proc", fs_root_node);
        put_file_in_dir(dev, fs_root_node);
        put_file_in_dir(bin, fs_root_node);
        put_file_in_dir(proc, fs_root_node);

        // file_region = vmm_reserve(20 * 1024);

        // make all the tarfs files into files and put into directories

        dev_zero->read = dev_zero_read;
        dev_zero->permissions = USR_READ;
        strcpy(dev_zero->filename, "zero");

        put_file_in_dir(dev_zero, dev);

        /*
        ofd_stdin->node = dev_serial;
        ofd_stdout->node = dev_serial;
        ofd_stderr->node = dev_serial;
        */

        dev_serial->write = dev_serial_write;
        dev_serial->read = dev_serial_read;
        dev_serial->filetype = FT_TTY;
        dev_serial->permissions = USR_READ | USR_WRITE;
        emplace_ring(&dev_serial->ring, 128);
        dev_serial->tty = &serial_tty;
        strcpy(dev_serial->filename, "serial");
        list_init(&dev_serial->blocked_threads);

        put_file_in_dir(dev_serial, dev);

        dev_serial2->write = dev_serial_write;
        dev_serial2->read = dev_serial_read;
        dev_serial2->filetype = FT_TTY;
        dev_serial2->permissions = USR_READ | USR_WRITE;
        emplace_ring(&dev_serial2->ring, 128);
        dev_serial2->tty = &serial_tty2;
        strcpy(dev_serial2->filename, "serial2");
        list_init(&dev_serial2->blocked_threads);

        put_file_in_dir(dev_serial2, dev);

        struct tar_header *tar = initfs;
        vmm_map_range((uintptr_t)tar, (uintptr_t)tar - VMM_VIRTUAL_OFFSET,
                      initfs_len + 0x1000, PAGE_PRESENT);
        while (tar->filename[0]) {
                size_t len = tar_convert_number(tar->size);

                char *filename = tar->filename;
                void *file_content = ((char *)tar) + 512;
                struct file *new_node =
                        make_tar_file(filename, len, file_content);
                put_file_in_dir(new_node, bin);

                uintptr_t next_tar = (uintptr_t)tar;
                next_tar += len + 0x200;
                next_tar = round_up(next_tar, 512);

                tar = (void *)next_tar;
        }

        printf("vfs: filesystem initialized\n");
}

void vfs_print_tree(struct file *root, int indent) {
        for (int i=0; i<indent; i++) {
                printf("  ");
        }
        printf("+ '%s' ", root->filename);
        printf(root->permissions & USR_READ ? "r" : "-");
        printf(root->permissions & USR_WRITE ? "w" : "-");
        printf(root->permissions & USR_EXEC ? "x" : "-");
        printf("\n");

        if (root->filetype == FT_DIRECTORY) {
                struct file *ch;
                list_foreach(&root->children, ch, directory_siblings) {
                        vfs_print_tree(ch, indent + 1);
                }
        }
}

