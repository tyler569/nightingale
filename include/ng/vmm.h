#pragma once
#ifndef NG_VMM_H
#define NG_VMM_H

#include <sys/cdefs.h>

enum fault_result {
    FAULT_CRASH,
    FAULT_CONTINUE,
};

#if X86
#include <x86/vmm.h>
#endif

void *vmm_reserve(size_t);
void *vmm_mapobj(void *, size_t);
void *vmm_mapobj_i(uintptr_t, size_t);
uintptr_t vmm_mapobj_iwi(uintptr_t, size_t);
void *high_vmm_reserve(size_t);

#endif // NG_VMM_H
