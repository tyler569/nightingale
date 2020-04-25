
#pragma once
#ifndef NG_MULTIBOOT_H
#define NG_MULTIBOOT_H

#include <basic.h>
#include <ng/multiboot2.h>
#include <ng/pmm.h>

void mb_init(uintptr_t mb_info);


size_t mb_length(void);
phys_addr_t mb_phy_base(void);
phys_addr_t mb_phy_end(void);
phys_addr_t mb_init_phy_base(void);
phys_addr_t mb_init_phy_end(void);

void *mb_find_tag_of_type(int tag_type);

char *mb_cmdline(void);
char *mb_bootloader(void);
void *mb_elf_tag(void);
void *mb_acpi_rsdp(void);

struct initfs_info {
        uintptr_t base;
        uintptr_t end;
};

struct initfs_info mb_initfs_info(void);
multiboot_tag_mmap *mb_mmap(void);

void mb_mmap_print(void);
size_t mb_mmap_total_usable(void);
void mb_mmap_enumerate(void (*cb) (uintptr_t, uintptr_t, int));

#endif // NG_MULTIBOOT_H

