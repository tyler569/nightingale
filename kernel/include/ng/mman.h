#pragma once

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/mman.h>

BEGIN_DECLS

enum physical_region_type {
    prt_memory,
    prt_kernel,
    prt_acpi,
    prt_bootloader,
    prt_reserved,
};

struct physical_region {
    uintptr_t base;
    size_t len;
    enum physical_region_type type;
};

void init_pmm();

END_DECLS
