#pragma once

#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

struct stat {
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    nlink_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    dev_t st_rdev;
    off_t st_size;
    blksize_t st_blksize;
    blkcnt_t st_blocks;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
};

int fstat(int fd, struct stat *buf);
int stat(const char *path, struct stat *buf);
int statat(int atfd, const char *path, struct stat *buf);

int chmod(const char *pathname, mode_t mode);
int chmodat(int atfd, const char *pathname, mode_t mode);
int fchmod(int fd, mode_t mode);

int mkdir(const char *path, mode_t mode);
int mkdirat(int atfd, const char *path, mode_t mode);

int mknod(const char *path, mode_t mode, dev_t dev);
int mknodat(int atfd, const char *path, mode_t mode, dev_t dev);

int mkfifo(const char *path, mode_t mode);
int mkfifoat(int atfd, const char *path, mode_t mode);

END_DECLS

