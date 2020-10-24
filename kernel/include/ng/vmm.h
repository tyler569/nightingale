#pragma once
#ifndef NG_VMM_H
#define NG_VMM_H

#include <basic.h>

#if X86
#include <ng/x86/vmm.h>
#endif

void *vmm_reserve(size_t);
void *high_vmm_reserve(size_t);

#endif // NG_VMM_H
