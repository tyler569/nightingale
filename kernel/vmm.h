
#pragma once
#ifndef NIGHTINGALE_ARCH_VMM_H
#define NIGHTINGALE_ARCH_VMM_H

#include <basic.h>
#include "arch/x86/vmm64.h" // change for diff. compilations

uintptr_t vmm_virt_to_phy(uintptr_t vma);
bool vmm_map(uintptr_t vma, uintptr_t pma, int flags);
void vmm_map_range(uintptr_t vma, uintptr_t pma, size_t len, int flags);
bool vmm_edit_flags(uintptr_t vma, int flags);

void vmm_create_unbacked(uintptr_t vma, int flags);
void vmm_create_unbacked_range(uintptr_t vma, size_t len, int flags);

int vmm_fork();

#endif

