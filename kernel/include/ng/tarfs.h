#pragma once
#ifndef NG_TARFS_H
#define NG_TARFS_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>

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

uint64_t tar_convert_number(char *num);
void tarfs_print_all_files(struct tar_header *tar);
void *tarfs_get_file(struct tar_header *tar, const char *filename);
size_t tarfs_get_len(struct tar_header *tar, const char *filename);

#endif // NG_TARFS_H
