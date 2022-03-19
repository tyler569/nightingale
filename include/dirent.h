#pragma once
#ifndef _DIRENT_H_
#define _DIRENT_H_

#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

struct dirent {
    ino_t d_ino;
    off_t d_off;
    unsigned short d_reclen;
    unsigned short d_mode;
    unsigned char d_type;
    char d_name[256];
};

#ifndef __kernel__
ssize_t getdents(int fd, struct dirent *buf, size_t size);
ssize_t readdir(int fd, struct dirent *buf, size_t size);
#endif

#endif // _DIRENT_H_
