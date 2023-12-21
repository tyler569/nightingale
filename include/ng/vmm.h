#pragma once
#ifndef NG_VMM_H
#define NG_VMM_H

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

enum fault_result {
    FR_OK,
    FR_NO_PAGE_TABLE,
    FR_NO_PAGE,
    FR_ILLEGAL_RESERVED,
    FR_STACK_GUARD_VIOLATION,
    FR_UNKNOWN_REASON,
};

#ifdef __x86_64__
#include <ng/x86/vmm.h>
#endif

BEGIN_DECLS

void *vmm_reserve(size_t);
const char *vmm_fault_result(enum fault_result result);

END_DECLS

#endif // NG_VMM_H
