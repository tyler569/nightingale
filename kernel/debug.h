
#pragma once
#ifndef NIGHTINGALE_DEBUG_H
#define NIGHTINGALE_DEBUG_H

#include <basic.h>
#include <print.h>

#ifdef DEBUG

// only debug if DEBUG defined before import

#define DEBUG_PRINTF(fmt, ...) \
    do { printf("[DEBUG] " fmt, ## __VA_ARGS__); } while (0)

#else // !DEBUG

#define DEBUG_PRINTF(...)

#endif // DEBUG

#define WARN_PRINTF(fmt, ...) \
    do { printf("[WARN!] " fmt, ## __VA_ARGS__); } while (0)


int backtrace_from_here(int max_frames);
int bt_test(int x);
int backtrace_from(uintptr_t rbp_, int max_frames);

#endif

