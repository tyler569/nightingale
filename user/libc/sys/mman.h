
#ifndef _SYS_MMAN_H_
#define _SYS_MMAN_H_

#include <ng/basic.h>

enum {
    PROT_READ = 1,
    PROT_WRITE = 2,
    PROT_EXEC = 4,
    PROT_NONE = 0,
};

enum {
    MAP_SHARED = 1,
    MAP_PRIVATE = 2,
};

enum {
    MAP_32BIT = 4,
    MAP_ANONYMOUS = 8,
    // others
};

void* mmap(void* addr, size_t len, int prot, int flags, int fd, off_t off);
int munmap(void* addr, size_t len);

#endif

