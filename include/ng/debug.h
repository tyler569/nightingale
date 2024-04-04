#pragma once

#include <assert.h>
#include <ng/cpu.h>
#include <stdio.h>
#include <sys/cdefs.h>

#ifdef DEBUG

#define do_debug true
#define DEBUG_PRINTF(...) \
    do { \
        printf("[DEBUG] " __VA_ARGS__); \
    } while (0)

#else // !DEBUG

#define do_debug false
#define DEBUG_PRINTF(...)

#endif // DEBUG

#define WARN_PRINTF(...) \
    do { \
        printf("[WARN!] " __VA_ARGS__); \
    } while (0)

#define UNREACHABLE() assert("not reachable" && 0)

#ifdef __x86_64__
#define HIGHER_HALF 0x800000000000
#else
#define HIGHER_HALF 0x80000000
#endif

BEGIN_DECLS

void backtrace(uintptr_t bp, uintptr_t ip,
    void (*callback)(uintptr_t bp, uintptr_t ip, void *), void *);
void backtrace_from_frame(interrupt_frame *);

int dump_mem(void *ptr, size_t len);
int hexdump(size_t len, char ptr[len]);

__NOINLINE void break_point();

END_DECLS

