#pragma once
#ifndef NG_MMAN_H
#define NG_MMAN_H

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/mman.h>

struct mem_region {
    uintptr_t base;
    size_t size;
    struct file *file;
    off_t offset;
    int prot;
    int flags;
};

#endif // NG_MMAN_H
