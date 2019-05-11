
#pragma once
#ifndef NIGHTINGALE_DEBUG_H
#define NIGHTINGALE_DEBUG_H

#include <ng/basic.h>
#include <ng/print.h>

#ifdef DEBUG

#define do_debug true
#define DEBUG_PRINTF(...)                                                      \
        do {                                                                   \
                printf("[DEBUG] " __VA_ARGS__);                                \
        } while (0)

#else // !DEBUG

#define do_debug false
#define DEBUG_PRINTF(...)

#endif // DEBUG

#define WARN_PRINTF(...)                                                       \
        do {                                                                   \
                printf("[WARN!] " __VA_ARGS__);                                \
        } while (0)

int backtrace_from_here(int max_frames);
int bt_test(int x);
int backtrace_from(uintptr_t bp, int max_frames);
void backtrace_from_with_ip(uintptr_t bp, int max_frames, uintptr_t ip);

int dump_mem(void *ptr, size_t len);

#endif
