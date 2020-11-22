#include <basic.h>
#include <linker/elf.h>
#include <stdlib.h>
#include <string.h>

// host-specific
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const char *ngk_data[] = {
    "Hello",
    "World",
};

static int ngk_0(const char *str) {
    (void)str;
}

int ngk_1(int a) {
    printf("Hello World from ngk_1(%i)\n", a);
    return a + 1;
}

int ngk_2(int a) {
    ngk_0("Hello World");
    return a - 2;
}

int ngk_3(int a) {
    return a + ngk_2(a);
}

int main() {
    struct stat sb;
    void *mod_load = (void *)0x800000;

    int fd = open("./mod.o", O_RDONLY);
    if (fd < 0) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    fstat(fd, &sb);

    void *mod_elf =
        mmap(mod_load, sb.st_size, PROT_READ | PROT_EXEC, MAP_PRIVATE, fd, 0);

    if ((long)mod_elf == -1) {
        perror("mmap()");
        exit(EXIT_FAILURE);
    }
    if (mod_elf != mod_load) {
        printf("kernel didn't load us to the right place ;-;\n");
        printf(" wanted: %p, got: %p\n", mod_load, mod_elf);
        exit(EXIT_FAILURE);
    }

    struct elfinfo ei = elf_info(mod_elf);

    size_t fn_off = elf_get_sym_off(&ei, "init_module");
    int (*init_module)(int) = (void *)((char *)mod_elf + fn_off);
    printf("init_module is at %p\n", init_module);

    init_module(10);

    printf("Hello World after module initialization\n");
}
