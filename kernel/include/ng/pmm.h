
#pragma once
#ifndef NIGHTINGALE_PMM_ALLOC_H
#define NIGHTINGALE_PMM_ALLOC_H

#include <stddef.h>
#include <stdint.h>

void pmm_allocator_init(uintptr_t first);
uintptr_t pmm_allocate_page();
void pmm_free_page(uintptr_t vmm);
uintptr_t pmm_allocate_contiguous(int count);

extern uint16_t *pmm_memory_map;
extern size_t pmm_memory_map_len;

#define PMM_MAP_NONE 0x0000
#define PMM_MAP_KERNEL 0x1000
#define PMM_MAP_USER 0x2000
#define PMM_MAP_VMM 0x3000
#define PMM_MAP_MULTIPLE 0xE000
#define PMM_MAP_RESERVED 0xF000 // things to never be touched
#define PMM_MAP_NOPHY 0xFFFF

void pmm_settype(uintptr_t pma, int type);
void pmm_setref(uintptr_t pma, int refcnt);
int pmm_type(uintptr_t pma);
int pmm_getref(uintptr_t pma);
int pmm_incref(uintptr_t pma);
int pmm_decref(uintptr_t pma);

#endif
