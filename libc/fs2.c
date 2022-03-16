#define FCNTL_NO_OPEN

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int __ng_openat(int fd, const char *path, int flags, int mode);

int open(const char *filename, int flags, int mode) {
    return __ng_openat(AT_FDCWD, filename, flags, mode);
}

off_t seek(int fd, off_t offset, int whence) {
    return lseek(fd, offset, whence);
}

int chmod(const char *path, int mode) {
    return chmodat(AT_FDCWD, path, mode);
}

ssize_t readdir(int fd, struct ng_dirent *buf, size_t count) {
    return getdents(fd, buf, count);
}

int unlink(const char *path) {
    return unlinkat(AT_FDCWD, path);
}

