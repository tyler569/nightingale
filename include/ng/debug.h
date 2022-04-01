#pragma once
#ifndef NG_DEBUG_H
#define NG_DEBUG_H

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

#if X86_64
#define HIGHER_HALF 0x800000000000
#else
#define HIGHER_HALF 0x80000000
#endif

void backtrace(uintptr_t bp, uintptr_t ip,
    void (*callback)(uintptr_t bp, uintptr_t ip, void *), void *);
void backtrace_from_here(void);
void backtrace_from_with_ip(uintptr_t bp, uintptr_t ip);
void print_perf_trace(uintptr_t bp, uintptr_t ip);

int dump_mem(void *ptr, size_t len);
int hexdump(size_t len, char ptr[len]);

__NOINLINE void break_point();

#endif // NG_DEBUG_H
