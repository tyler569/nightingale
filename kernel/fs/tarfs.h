
#ifndef NIGHTINGALE_FS_TARFS_H
#define NIGHTINGALE_FS_TARFS_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <vector.h>
#include <syscall.h>

struct tar_header {
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
};

uint64_t tar_number_convert(char *num);

#endif
