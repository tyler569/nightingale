
#pragma once

#include <stddef.h>

#include <multiboot.h>
#include <kernel/cpu.h>

#define PMM_STACK_TOP (uintptr_t *)0xFFC00000

#define PAGE_UNALLOCATED -10

void pmm_init(multiboot_info_t *mbdata);

int pmm_alloc_page(uintptr_t pma);
int pmm_free_page(uintptr_t pma);

void pmm_do_page_fault(struct regs *r);

int pmm_raw_map_page(uintptr_t vma, uintptr_t pma, uint32_t flags);

