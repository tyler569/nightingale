#pragma once
#ifndef NG_MULTIBOOT_H
#define NG_MULTIBOOT_H

#include <ng/multiboot2.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

BEGIN_DECLS

void mb_init(uintptr_t mb_info);
void *mb_find_tag_iof_type(int tag_type);
char *mb_cmdline(void);
char *mb_bootloader(void);
void *mb_elf_tag(void);
void *mb_acpi_rsdp();

struct initfs_info {
    uintptr_t base;
    uintptr_t top;
};

struct initfs_info mb_initfs_info(void);
void mb_mmap_print(void);
size_t mb_mmap_total_usable(void);
void mb_mmap_enumerate(void (*cb)(uintptr_t, uintptr_t, int));

END_DECLS

#endif // NG_MULTIBOOT_H
