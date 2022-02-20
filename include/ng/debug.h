#pragma once
#ifndef NG_DEBUG_H
#define NG_DEBUG_H

#include <basic.h>
#include <stdio.h>

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

#define gassert(assertion) \
    do { \
        if (!(assertion)) { \
            printf( \
    "[ASSERT] '" #assertion "' @ " __FILE__ \
    ":" QUOTE(__LINE__) "\n" \
            ); \
            panic_gbt(); \
            __builtin_unreachable(); \
        } \
    } while (0)

void backtrace(
    uintptr_t bp,
    uintptr_t ip,
    void (*callback)(uintptr_t bp, uintptr_t ip)
);
void backtrace_from_here(void);
void backtrace_from_with_ip(uintptr_t bp, uintptr_t ip);
void print_perf_trace(uintptr_t bp, uintptr_t ip);

int dump_mem(void *ptr, size_t len);
int hexdump(size_t len, char ptr[len]);

__NOINLINE void break_point();

#endif // NG_DEBUG_H
