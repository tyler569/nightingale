#include <basic.h>
#include <linker/elf.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// host-specific
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


void *load_file(const char *filename, int write) {
    struct stat sb;
    int fd = open(filename, O_RDWR);

    if (fd < 0) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    fstat(fd, &sb);

    void *map =
        mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if ((long)map == -1) {
        perror("mmap()");
        exit(EXIT_FAILURE);
    }

    return map;
}

int main() {
    const char *ngk = "./demo";
    const char *mod = "./mod.o";

    void *ngk_elf = load_file(ngk, 0);
    void *mod_elf = load_file(mod, 1);

    struct elfinfo ei_ngk = elf_info(ngk_elf);
    struct elfinfo ei_mod = elf_info(mod_elf);

    elf_resolve_symbols(&ei_ngk, &ei_mod);

    printf("\n\n == relocating object ==\n\n");

    elf_relocate_object(&ei_mod, 0x800000);

    return 0;
}
