
#pragma once
#ifndef NG_PMM_H
#define NG_PMM_H

#include <stddef.h>
#include <stdint.h>
#include <ng/multiboot2.h>
#include <ng/mutex.h>
#include <ng/vmm.h>
#include <nc/list.h>

typedef uintptr_t phys_addr_t;

#define PM_NULL (phys_addr_t)0

enum pm_state {
        PM_NONE,
        PM_HWRESERVED,
        PM_KERNEL,
        PM_MULTIBOOT,
        PM_INITFS,

        PM_FREE,
        PM_ONEPAGE,
        PM_ONEPAGE_FULL,
        PM_CONTIGUOUS,

        PM_EVILHACK, // see my novel in main.c
};

struct pm_region {
        phys_addr_t base;
        phys_addr_t top;
        int pages;
        int pages_free;
        enum pm_state state;
        list_node node;
        struct bitmap *bitmap;
};

extern list pmm_reserved_regions;
extern list pmm_free_regions;
extern list pmm_onepage_regions;

/* OLD
void pmm_allocator_init(uintptr_t first);
uintptr_t pmm_allocate_page();
void pmm_free_page(uintptr_t vmm);
uintptr_t pmm_allocate_contiguous(int count);
*/

void pm_mb_init(multiboot_tag_mmap *mmap);
void pm_reserve(phys_addr_t base, phys_addr_t top, enum pm_state reason);

phys_addr_t pm_alloc_page(void);
phys_addr_t pm_alloc_contiguous(int pages);
void pm_free_page(phys_addr_t page);
void pm_free_contiguous(phys_addr_t base, int pages);

void pm_dump_regions(void);

#endif // NG_PMM_H

