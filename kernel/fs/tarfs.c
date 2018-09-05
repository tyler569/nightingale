
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <print.h>
#include <fs/vfs.h>
#include "tarfs.h"

uint64_t tar_convert_number(char *num) {
    size_t len = strlen(num);
    uint64_t value = 0;
    
    for (size_t place=0; place<=len; place += 1) {
        value += (num[place] - '0') << ((len - 1 - place) * 3);
    }

    return value;
}

void tarfs_print_all_files(struct tar_header *tar) {
    void *top = tar;
    while (tar->filename[0]) {
        size_t len = tar_convert_number(tar->size);
        printf("%s (%lub)\n", tar->filename, len);

        uintptr_t next_tar = (uintptr_t)tar;
        next_tar += ((len / 512) + 2) * 512;

        // if (next_tar % 512)
        //     next_tar += 512;

        tar = (void *)next_tar;
        // printf("next @ %p\n", (void *)tar - top);
        // printf("next  '%c'\n", *(char *)tar);
    }
    printf("done.\n");
}

void *tarfs_get_file(struct tar_header *tar, char *filename) {
    while (tar->filename[0]) {
        if (strcmp(tar->filename, filename) == 0) {
            return (void *)tar + 512;
        }

        size_t len = tar_convert_number(tar->size);

        uintptr_t next_tar = (uintptr_t)tar;
        next_tar += ((len / 512) + 2) * 512;

        // if (next_tar % 512)
        //     next_tar += 512;

        tar = (void *)next_tar;
    }

    return NULL;
}

