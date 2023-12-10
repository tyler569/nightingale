#include <ng/common.h>
#include <ng/string.h>
#include <ng/tarfs.h>
#include <stddef.h>
#include <stdio.h>

uint64_t tar_convert_number(char *num)
{
    uint64_t value = 0;

    for (size_t place = 0; num[place]; place += 1) {
        uint64_t part = num[place] - '0';
        value <<= 3;
        value += part;
    }

    return value;
}

void tarfs_print_all_files(tar_header *tar)
{
    while (tar->filename[0]) {
        size_t len = tar_convert_number(tar->size);
        printf("%s (%lub)\n", tar->filename, len);

        auto next_tar = (uintptr_t)tar;
        next_tar += len + 0x200;
        next_tar = ROUND_UP(next_tar, 512);
        tar = (tar_header *)next_tar;
    }
    printf("done.\n");
}

void *tarfs_get_file(tar_header *tar, const char *filename)
{
    while (tar->filename[0]) {
        if (strcmp(tar->filename, filename) == 0)
            return (char *)tar + 512;

        size_t len = tar_convert_number(tar->size);

        auto next_tar = (uintptr_t)tar;
        next_tar += len + 0x200;
        next_tar = ROUND_UP(next_tar, 512);
        tar = (tar_header *)next_tar;
    }

    return NULL;
}

size_t tarfs_get_len(tar_header *tar, const char *filename)
{
    while (tar->filename[0]) {
        size_t len = tar_convert_number(tar->size);

        if (strcmp(tar->filename, filename) == 0)
            return len;

        auto next_tar = (uintptr_t)tar;
        next_tar += len + 0x200;
        next_tar = ROUND_UP(next_tar, 512);
        tar = (tar_header *)next_tar;
    }

    return 0;
}
