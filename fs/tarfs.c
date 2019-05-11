
#include <ng/basic.h>
#include <ng/print.h>
#include <ng/string.h>
#include <fs/tarfs.h>
#include <ng/fs.h>
#include <stddef.h>
#include <stdint.h>

uint64_t tar_convert_number(char *num) {
        size_t len = strlen(num);
        uint64_t value = 0;

        for (size_t place = 0; place < len; place += 1) {
                uint64_t part = num[place] - '0';
                uint64_t shift = (len - 1 - place) * 3;
                value += part << shift;
        }

        return value;
}

void tarfs_print_all_files(struct tar_header *tar) {
        void *top = tar;
        while (tar->filename[0]) {
                size_t len = tar_convert_number(tar->size);
                printf("%s (%lub)\n", tar->filename, len);

                uintptr_t next_tar = (uintptr_t)tar;
                next_tar += len + 0x200;
                next_tar = round_up(next_tar, 512);
                // potentially need to add 512 more if it was aligned to begin
                // with? fragile

                tar = (void *)next_tar;
                // printf("next @ %p\n", (void*)tar - top);
                // printf("next  '%c'\n", *(char*)tar);
        }
        printf("done.\n");
}

void *tarfs_get_file(struct tar_header *tar, const char *filename) {
        while (tar->filename[0]) {
                if (strcmp(tar->filename, filename) == 0) {
                        return (char *)tar + 512;
                }

                size_t len = tar_convert_number(tar->size);

                // COPYPASTE from above print_all_files
                uintptr_t next_tar = (uintptr_t)tar;
                next_tar += len + 0x200;
                next_tar = round_up(next_tar, 512);
                // potentially need to add 512 more if it was aligned to begin
                // with? fragile

                tar = (struct tar_header *)next_tar;
        }

        return NULL;
}

size_t tarfs_get_len(struct tar_header *tar, const char *filename) {
        while (tar->filename[0]) {
                size_t len = tar_convert_number(tar->size);

                if (strcmp(tar->filename, filename) == 0) {
                        return len;
                }

                // COPYPASTE from above print_all_files
                uintptr_t next_tar = (uintptr_t)tar;
                next_tar += len + 0x200;
                next_tar = round_up(next_tar, 512);
                // potentially need to add 512 more if it was aligned to begin
                // with? fragile

                tar = (struct tar_header *)next_tar;
        }

        return 0;
}

