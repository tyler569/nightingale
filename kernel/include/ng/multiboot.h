
#pragma once
#ifndef NG_MULTIBOOT_H
#define NG_MULTIBOOT_H

#include <basic.h>
#include <ng/multiboot2.h>

void mb_init(uintptr_t mb_info);

void *mb_find_tag_iof_type(int tag_type);

char *mb_cmdline(void);
char *mb_bootloader(void);
void *mb_elf_tag(void);
void *mb_acpi_rsdp();

struct initfs_info {
        uintptr_t base;
        uintptr_t end;
};

struct initfs_info mb_initfs_info(void);

void mb_mmap_print(void);
size_t mb_mmap_total_usable(void);
void mb_mmap_enumerate(void (*cb) (uintptr_t, uintptr_t, int));

#endif // NG_MULTIBOOT_H

