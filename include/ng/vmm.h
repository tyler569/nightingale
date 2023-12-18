#pragma once
#ifndef NG_VMM_H
#define NG_VMM_H

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

BEGIN_DECLS

enum fault_result {
    FAULT_CRASH,
    FAULT_CONTINUE,
};

#ifdef __x86_64__
#include <ng/x86/vmm.h>
#endif

void *vmm_reserve(size_t);

END_DECLS

#endif // NG_VMM_H
