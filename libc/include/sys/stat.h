#pragma once
#ifndef _SYS_STAT_H_
#define _SYS_STAT_H_

#include <sys/types.h>

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

#endif // _SYS_STAT_H_
