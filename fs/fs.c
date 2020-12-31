#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ng/dmgr.h>
#include <ng/fs.h>
#include <ng/string.h>
#include <ng/syscall.h>
#include <ng/tarfs.h>
#include <ng/thread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

#define is_error(R) ((intptr_t)(R) < 0 && (intptr_t)(R) > -0x1000)

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
    .file =
        {
            .type = FT_DIRECTORY,
            .mode = USR_READ | USR_WRITE,
            .uid = 0,
            .gid = 0,
        },
};

struct file *fs_root;

#define FS_NODE_BOILER(fd, perm)                                               \
    struct open_file *ofd = dmgr_get(&running_process->fds, fd);               \
    if (ofd == NULL) return -EBADF;                                            \
    if ((ofd->mode & perm) != perm) return -EPERM;                             \
    struct file *node = ofd->file;

struct file_pair {
    struct file *dir, *file;
};

struct file_pair fs_resolve(struct file *root, const char *path) {
    struct file *dir = root, *file = NULL, *tmp;
    const char *cursor = path;
    char buf[128] = {0};

    if (cursor[0] == '/') {
        dir = fs_root;
        cursor++;
        if (!cursor[0]) return (struct file_pair){fs_root, fs_root};
    }

    while (cursor[0]) {
        cursor = strcpyto(buf, cursor, '/');
        if (cursor[0] == '/') cursor++;
        // support foo//bar, just keep going.
        if (!buf[0] && cursor[0]) continue;
        // foo/
        if (!buf[0] && !cursor[0]) break;

        if (file && file->type == FT_DIRECTORY) dir = file;
        if (!dir || !dir->ops->child) {
            if (strchr(cursor, '/')) return (struct file_pair){NULL, NULL};
            // If there are no more path delimeters in the string, we
            // did find a directory, but the file doesn't exist. We can
            // still return from directory_of I think
            return (struct file_pair){dir, NULL};
        }
        file = dir->ops->child(dir, buf);
    }
    return (struct file_pair){dir, file};
}

struct file *fs_resolve_relative_path(struct file *root, const char *path) {
    return fs_resolve(root, path).file;
}

struct file *fs_resolve_directory_of(struct file *root, const char *path) {
    struct file *dir = fs_resolve(root, path).dir;

    if (dir->type == FT_DIRECTORY) {
        return dir;
    } else {
        printf("WARN: directory_of ended up with no directory");
        return NULL;
    }
}

struct file *fs_path(const char *path) {
    return fs_resolve_relative_path(NULL, path);
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

void last_name(char *buf, size_t len, const char *filename) {
    const char *last = filename;
    const char *tmp;
    while ((tmp = strchr(last, '/')) && tmp[1]) { last = tmp + 1; }
    strncpyto(buf, last, len, '/');
}

sysret do_open(struct file *file, const char *basename, int flags, int mode) {
    assert(file);
    assert(file->ops);

    if (!(flags & O_CREAT)) {
        if ((flags & O_RDONLY) && !(file->mode & USR_READ)) return -EPERM;

        if ((flags & O_WRONLY) && !(file->mode & USR_WRITE)) return -EPERM;

        if ((flags & O_WRONLY && flags & O_TRUNC)) {
            // is that it?
            file->len = 0;
        }

        // mode argument is ignored
        mode = 0;
        if (flags & O_RDONLY) mode |= USR_READ;
        if (flags & O_WRONLY) mode |= USR_WRITE;
    }

    struct open_file *new_open_file = zmalloc(sizeof(struct open_file));
    new_open_file->file = file;
    new_open_file->mode = mode;
    new_open_file->off = 0;
    file->refcnt++;

    if (file->ops->open) file->ops->open(new_open_file, basename);

    return dmgr_insert(&running_process->fds, new_open_file);
}

sysret do_sys_open(struct file *root, char *filename, int flags, int mode) {
    struct file *node = fs_resolve_relative_path(root, filename);

    if (!node) {
        if (flags & O_CREAT) {
            int use_mode = 0666; // umask
            if (mode != 0) use_mode = mode;
            node = create_file(root, filename, use_mode);
            mode = USR_READ | USR_WRITE;
            if is_error (node) return (intptr_t)node;
        } else {
            return -ENOENT;
        }
    }

    return do_open(node, basename(filename), flags, mode);
}

sysret sys_open(char *filename, int flags, int mode) {
    return do_sys_open(running_thread->cwd, filename, flags, mode);
}

sysret sys_openat(int fd, char *filename, int flags, int mode) {
    FS_NODE_BOILER(fd, 0);
    if (node->type != FT_DIRECTORY) return -EBADF;

    return do_sys_open(node, filename, flags, mode);
}

sysret do_close_open_file(struct open_file *ofd) {
    struct file *node = ofd->file;

    DECREF(node);
    if (node->ops->close) node->ops->close(ofd);

    // if (ofd->basename) free(ofd->basename);
    free(ofd);
    return 0;
}

sysret sys_close(int fd) {
    FS_NODE_BOILER(fd, 0);
    dmgr_drop(&running_process->fds, fd);
    return do_close_open_file(ofd);
}

sysret sys_unlink(const char *name) {
    struct file *dir = fs_resolve_directory_of(running_thread->cwd, name);
    if (!dir) return -ENOENT;
    if (dir->type != FT_DIRECTORY) return -ENOTDIR;
    if (!dir->ops->child) return -ENOTDIR;
    if (!(dir->mode & USR_WRITE)) return -EPERM;

    const char *dirname = basename(name);
    struct file *file = dir->ops->child(dir, dirname);
    if (!file) return -ENOENT;
    if (!(file->mode & USR_WRITE)) return -EPERM;
    file = remove_dir_file(dir, dirname); // does a decref

    if (file->refcnt == 0) {
        if (file->ops->destroy) file->ops->destroy(file);
    }
    return 0;
}

sysret sys_read(int fd, void *data, size_t len) {
    FS_NODE_BOILER(fd, USR_READ);

    if (node->type == FT_DIRECTORY) return -EISDIR;

    ssize_t value;
    while ((value = node->ops->read(ofd, data, len)) == -1) {
        if (node->flags & FILE_NONBLOCKING) return -EWOULDBLOCK;

        if (node->signal_eof) {
            node->signal_eof = 0;
            return 0;
        }

        wq_block_on(&node->readq);
    }

    return value;
}

sysret sys_write(int fd, const void *data, size_t len) {
    FS_NODE_BOILER(fd, USR_WRITE);

    return node->ops->write(ofd, data, len);
}

sysret sys_readdir(int fd, struct ng_dirent *buf, size_t count) {
    struct open_file *ofd = dmgr_get(&running_process->fds, fd);
    if (!ofd) return -EBADF;
    struct file *file = ofd->file;

    return file->ops->readdir(ofd, buf, count);
}

struct open_file *clone_open_file(struct open_file *ofd) {
    struct open_file *nfd = malloc(sizeof(struct open_file));
    memcpy(nfd, ofd, sizeof(struct open_file));
    // if (ofd->basename) nfd->basename = strdup(ofd->basename);
    ofd->file->refcnt += 1;
    if (ofd->file->ops->clone) ofd->file->ops->clone(ofd, nfd);
    return nfd;
}

sysret sys_dup2(int oldfd, int newfd) {
    if (oldfd == newfd) return 0;
    struct open_file *ofd = dmgr_get(&running_process->fds, oldfd);
    if (!ofd) return -EBADF;

    struct open_file *nfd = dmgr_get(&running_process->fds, newfd);
    if (nfd) do_close_open_file(nfd);
    nfd = clone_open_file(ofd);
    dmgr_set(&running_process->fds, newfd, nfd);

    return newfd;
}

sysret sys_seek(int fd, off_t offset, int whence) {
    if (whence > SEEK_END || whence < SEEK_SET) return -EINVAL;

    struct open_file *ofd = dmgr_get(&running_process->fds, fd);
    if (ofd == NULL) return -EBADF;

    struct file *node = ofd->file;
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

    if (timeout > 0) return -ETODO;

    for (int i = 0; i < nfds; i++) {
        if (fds[i].fd < 0) continue;

        struct open_file *ofd = dmgr_get(&running_process->fds, fds[i].fd);
        if (ofd == NULL) return -EBADF;
        struct file *node = ofd->file;

        if (!node) return -EBADF;

        if (node->type != FT_TTY) {
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

sysret do_chmod(struct file *file, mode_t mode) {
    file->mode = mode;
    return 0;
}

sysret sys_chmod(const char *path, mode_t mode) {
    struct file *file = fs_resolve_relative_path(running_thread->cwd, path);
    if (!file) return -ENOENT;
    return do_chmod(file, mode);
}

sysret sys_fchmod(int fd, mode_t mode) {
    struct open_file *ofd = dmgr_get(&running_process->fds, fd);
    if (!ofd) return -EBADF;
    struct file *file = ofd->file;
    return do_chmod(file, mode);
}

sysret do_stat(struct file *file, struct stat *statbuf) {
    statbuf->st_dev = 0;
    statbuf->st_ino = 0;
    statbuf->st_mode = file->mode;
    statbuf->st_nlink = file->refcnt;
    statbuf->st_uid = 0;
    statbuf->st_gid = 0;
    statbuf->st_rdev = 0;
    statbuf->st_size = file->len;
    statbuf->st_blksize = 512;
    statbuf->st_blocks = round_up(file->len, 512) / 512;
    statbuf->st_atime = 0;
    statbuf->st_mtime = 0;
    statbuf->st_ctime = 0;

    return 0;
}

sysret sys_fstat(int fd, struct stat *statbuf) {
    struct open_file *ofd = dmgr_get(&running_process->fds, fd);
    if (!ofd) return -EBADF;
    struct file *file = ofd->file;
    return do_stat(file, statbuf);
}

static void internal_fs_tree(struct file *root, int depth) {
    if (root->type != FT_DIRECTORY) return;

    struct directory_file *dir = (struct directory_file *)root;
    list_for_each(struct directory_node, node, &dir->entries, siblings) {
        for (int i = 0; i < depth; i++) printf("  ");

        printf("%s\n", node->name);
        if (node->name[0] != '.') {
            // Don't infinitely recurse the ".", ".."!
            internal_fs_tree(node->file, depth + 1);
        }
    }
}

void fs_tree() {
    internal_fs_tree(fs_root, 1);
}

// FIXME: what's this supposed to do?
void destroy_file(struct file *_) {}

struct file_ops dev_zero_ops = {
    .read = dev_zero_read,
};

struct file *dev_zero = &(struct file){
    .ops = &dev_zero_ops,
    .type = FT_CHARDEV,
    .mode = USR_READ,
};

struct file_ops dev_null_ops = {
    .read = dev_null_read,
    .write = dev_null_write,
};

struct file *dev_null = &(struct file){
    .ops = &dev_null_ops,
    .type = FT_CHARDEV,
    .mode = USR_READ | USR_WRITE,
};

void vfs_boot_file_setup(void) {
    wq_init(&dev_zero->readq);
    wq_init(&dev_serial.file.readq);
    wq_init(&dev_serial2.file.readq);
}

void vfs_init(uintptr_t initfs_len) {
    fs_root = &fs_root_node->file;
    fs_root_init();
    vfs_boot_file_setup();

    struct file *dev = make_directory(fs_root, "dev");
    make_directory(fs_root, "proc");

    add_dir_file(dev, dev_zero, "zero");
    add_dir_file(dev, dev_null, "null");
    add_dir_file(dev, &dev_serial.file, "serial");
    add_dir_file(dev, &dev_serial2.file, "serial2");

    struct tar_header *tar = initfs;
    uintptr_t tar_addr = (uintptr_t)tar;

    struct file *tar_file;
    uintptr_t next_tar;
    while (tar->filename[0]) {
        size_t len = tar_convert_number(tar->size);
        int mode = tar_convert_number(tar->mode);

        void *content = ((char *)tar) + 512;
        const char *filename = tar->filename;
        char name_buf[128];
        const char *base = basename(filename);

        struct file *directory = fs_resolve_directory_of(fs_root, filename);
        if (!directory) {
            printf("warning: can't place '%s' in file tree\n", filename);
            goto next;
        }

        if (tar->typeflag == REGTYPE || tar->typeflag == AREGTYPE) {
            tar_file = make_tar_file(base, mode, len, content);
            add_dir_file(directory, tar_file, base);
        } else if (tar->typeflag == DIRTYPE) {
            last_name(name_buf, 128, filename);
            // printf("make_directory(%p, \"%s\")\n", directory, name_buf);
            make_directory(directory, strdup(name_buf));
        } else {
            printf("warning: tar file of unknown type '%c' (%i)\n",
                   tar->typeflag, tar->typeflag);
        }

    next:
        next_tar = (uintptr_t)tar;
        next_tar += len + 0x200;
        next_tar = round_up(next_tar, 512);
        tar = (void *)next_tar;
    }

    // fs_tree();
    // struct file *test = fs_path("/usr/bin/init");
    // printf("%p\n", test);

    printf("vfs: filesystem initialized\n");
}
