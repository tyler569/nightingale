
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
    while (tar->filename[0]) {
        size_t len = tar_convert_number(tar->size);
        printf("%s (%lub)\n", tar->filename, len);

        uintptr_t next_tar = (uintptr_t)tar;
        next_tar += ((len / 512) + 2) * 512;

        if (next_tar % 512)
            next_tar += 512;

        tar = (void *)next_tar;
    }
    printf("done.\n");
}

