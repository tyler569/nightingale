
#include <basic.h>
#include <ng/fs.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <ng/dmgr.h>
#include <ng/ringbuf.h>
#include <ng/tarfs.h>
#include <assert.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

extern struct tar_header *initfs;

struct file *new_file_slot() {
        // there are pointers in here, zmalloc it
        return zmalloc(sizeof(struct file));
}

void free_file_slot(struct file *defunct) {
        free(defunct);
}

// should these not be static?
struct directory_file *fs_root_node = &(struct directory_file){
        .file = {
                .filetype = FT_DIRECTORY,
                .permissions = USR_READ | USR_WRITE,
                .uid = 0,
                .gid = 0,
        },
};

struct file *fs_root;

#define FS_NODE_BOILER(fd, perm) \
        struct open_file *ofd = dmgr_get(&running_process->fds, fd); \
        if (ofd == NULL) { return -EBADF; } \
        if ((ofd->flags & perm) != perm) { return -EPERM; } \
        struct file *node = ofd->node;

struct file *fs_resolve_relative_path(struct file *root, const char *filename) {
        struct file *node = root;

        if (!node || filename[0] == '/') {
                node = &fs_root_node->file;
                filename++;
        }

        char name_buf[MAX_FILENAME];

        while (filename && filename[0] && node) {
                if (node->filetype != FT_DIRECTORY)
                        break;
                filename = str_until(filename, name_buf, "/");
                node = dir_child(node, name_buf);
        }
        
        return node;
}

struct file *fs_path(const char *filename) {
        return fs_resolve_relative_path(NULL, filename);
}

const char *basename(const char *filename) {
        char *last;
        if (strchr(filename, '/')) {
                last = strrchr(filename, '/');
                return last + 1;
        } else {
                return filename;
        }
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

        assert(node->ops);

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
        new_open_file->basename = strdup(basename(filename));
        new_open_file->flags = mode;
        new_open_file->off = 0;
        node->refcnt++;

        if (node->ops->open) {
                node->ops->open(new_open_file);
        }

        return dmgr_insert(&running_process->fds, new_open_file);
}

sysret sys_open(char *filename, int flags, int mode) {
        return do_sys_open(running_thread->cwd, filename, flags, mode);
}

sysret sys_openat(int fd, char *filename, int flags, int mode) {
        FS_NODE_BOILER(fd, 0);
        if (node->filetype != FT_DIRECTORY)  return -EBADF;

        return do_sys_open(node, filename, flags, mode);
}

sysret do_close_open_file(struct open_file *ofd) {
        struct file *node = ofd->node;

        node->refcnt--;
        if (node->ops->close) {
                node->ops->close(ofd);
        }

        if (ofd->basename) {
                free(ofd->basename);
        }
        free(ofd);
        return 0;
}

sysret sys_close(int fd) {
        FS_NODE_BOILER(fd, 0);
        dmgr_drop(&running_process->fds, fd);
        return do_close_open_file(ofd);
}

sysret sys_read(int fd, void *data, size_t len) {
        FS_NODE_BOILER(fd, USR_READ);

        if (node->filetype == FT_DIRECTORY) {
                return -EISDIR;
        }

        ssize_t value;
        while ((value = node->ops->read(ofd, data, len)) == -1) {
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

        len = node->ops->write(ofd, data, len);
        return len;
}

sysret sys_readdir(int fd, struct ng_dirent *buf, size_t count) {
        struct open_file *ofd = dmgr_get(&running_process->fds, fd);
        if (!ofd) {
                return -EBADF;
        }
        struct file *file = ofd->node;

        return file->ops->readdir(ofd, buf, count);
}

struct open_file *clone_open_file(struct open_file *ofd) {
        struct open_file *nfd = malloc(sizeof(struct open_file));
        memcpy(nfd, ofd, sizeof(struct open_file));
        if (ofd->basename) {
                nfd->basename = strdup(ofd->basename);
        }
        ofd->node->refcnt += 1;
        if (ofd->node->ops->clone) {
                ofd->node->ops->clone(ofd, nfd);
        }
        return nfd;
}

sysret sys_dup2(int oldfd, int newfd) {
        struct open_file *ofd = dmgr_get(&running_process->fds, oldfd);
        if (!ofd)  return -EBADF;

        struct open_file *nfd = dmgr_get(&running_process->fds, newfd);
        if (nfd) {
                do_close_open_file(nfd);
        }
        nfd = clone_open_file(ofd);
        dmgr_set(&running_process->fds, newfd, nfd);

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
        if (node->ops->seek) {
                node->ops->seek(ofd, offset, whence);
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

                // FIXME: what file type was this intended for??
                // if (node->ring.len != 0) {
                //         fds[i].revents = POLLIN;
                //         return 1;
                // }
        }

        return 0;
}

void fs_tree(struct file *root, int depth) {
        if (root->filetype != FT_DIRECTORY) {
                return;
        }

        struct directory_file *dir = (struct directory_file *)root;
        list_for_each(struct directory_node, node, &dir->entries, siblings) {
                for (int i=0; i<depth; i++)  printf("  ");

                printf("%s\n", node->name);
                if (node->name[0] != '.') {
                        // Don't infinitely recurse the ".", ".."!
                        fs_tree(node->file, depth + 1);
                }
        }
}

// FIXME: what's this supposed to do?
void destroy_file(struct file *_) {}

struct file_ops dev_zero_ops = {
        .read = dev_zero_read,
};

struct file *dev_zero = &(struct file){
        .ops = &dev_zero_ops,
        .filetype = FT_CHARDEV,
        .permissions = USR_READ,
};

struct file_ops dev_null_ops = {
        .read = dev_null_read,
        .write = dev_null_write,
};

struct file *dev_null = &(struct file){
        .ops = &dev_null_ops,
        .filetype = FT_CHARDEV,
        .permissions = USR_READ | USR_WRITE,
};

void vfs_boot_file_setup(void) {
        list_init(&dev_zero->blocked_threads);
        list_init(&dev_serial.file.blocked_threads);
        list_init(&dev_serial2.file.blocked_threads);
}

void vfs_init(uintptr_t initfs_len) {
        fs_root = &fs_root_node->file;
        fs_root_init();
        vfs_boot_file_setup();

        struct file *dev = make_directory(fs_root, "dev");
        struct file *bin = make_directory(fs_root, "bin");
        struct file *proc = make_directory(fs_root, "proc");

        add_dir_file(dev, dev_zero, "zero");
        add_dir_file(dev, dev_null, "null");
        add_dir_file(dev, &dev_serial.file, "serial");
        add_dir_file(dev, &dev_serial2.file, "serial2");

        struct tar_header *tar = initfs;
        uintptr_t tar_addr = (uintptr_t)tar;

        struct file *tar_file;
        while (tar->filename[0]) {
                size_t len = tar_convert_number(tar->size);
                char *filename = tar->filename;
                void *content = ((char *)tar) + 512;
                tar_file = make_tar_file(filename, len, content);
                add_dir_file(bin, tar_file, filename);

                uintptr_t next_tar = (uintptr_t)tar;
                next_tar += len + 0x200;
                next_tar = round_up(next_tar, 512);
                tar = (void *)next_tar;
        }

        fs_tree(fs_root, 1);
        struct file *test = fs_path("/bin/init");
        // printf("%p\n", test);

        printf("vfs: filesystem initialized\n");
}
