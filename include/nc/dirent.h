
#pragma once
#ifndef _DIRENT_H_
#define _DIRENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>
#include <errno.h>

struct ng_dirent {
        enum filetype type;
        enum file_permission permissions;
        char filename[64];
};

ssize_t getdirents(int fd, struct ng_dirent *buf, ssize_t count);


#endif // _DIRENT_H_

