
#pragma once
#ifndef NG_VMM_H
#define NG_VMM_H

#include <ng/basic.h>

#if X86_64
#include <ng/x86/64/vmm.h>
#elif I686
#include <ng/x86/32/vmm.h>
#endif

void *vmm_reserve(size_t);
void *high_vmm_reserve(size_t);

// I guarantee these functions publically, and they are defined in the
// implementation-specific vmm.h
/*
#define PAGE_PRESENT
#define PAGE_WRITEABLE
#define PAGE_USERMODE
#define PAGE_ACCESSED
#define PAGE_DIRTY
#define PAGE_ISHUGE
#define PAGE_GLOBAL

#define PAGE_SIZE

uintptr_t vmm_virt_to_phy(uintptr_t vma);
uintptr_t vmm_resolve(uintptr_t vma);
bool vmm_map(uintptr_t vma, uintptr_t pma, int flags);
bool vmm_unmap(uintptr_t vma);
void vmm_map_range(uintptr_t vma, uintptr_t pma, size_t len, int flags);
bool vmm_edit_flags(uintptr_t vma, int flags);

void vmm_create(uintptr_t vma, int flags);
void vmm_create_unbacked(uintptr_t vma, int flags);
void vmm_create_unbacked_range(uintptr_t vma, size_t len, int flags);

int vmm_fork();
*/

#endif // NG_VMM_H

