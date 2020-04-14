
#ifdef __kernel__
#error "This <basic.h> should never be included by code integrated into ng"
#endif

#pragma once
#ifndef NIGHTINGALE_BASIC_H
#define NIGHTINGALE_BASIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define CAT_(x, y) x##y
#define CAT(x, y) CAT_(x, y)

#ifdef __x86_64__
#define X86_64 1
#elif defined(__i386__) || defined(__i686__)
#define I686 1
#endif

#ifdef __GNUC__
#define _packed __attribute__((packed))
#define _noreturn __attribute__((noreturn))
#define _used __attribute__((used))
#define noinline __attribute__((noinline))

#ifndef noreturn
#define noreturn __noreturn
#endif

#else
#error "Not __GNUC__ -- edit basic.h for your compiler"
#endif

// GCC stack smasking protection
extern uintptr_t __stack_chk_guard;
void __stack_chk_fail(void);

#define asm __asm__

static inline intptr_t max(intptr_t a, intptr_t b) {
        return (a > b) ? a : b;
}

static inline intptr_t min(intptr_t a, intptr_t b) {
        return (a < b) ? a : b;
}

static inline size_t umax(size_t a, size_t b) {
        return (a > b) ? a : b;
}

static inline size_t umin(size_t a, size_t b) {
        return (a < b) ? a : b;
}

static inline uintptr_t round_up(uintptr_t val, uintptr_t place) {
        return (val + place - 1) & ~(place - 1);
}

static inline uintptr_t round_down(uintptr_t val, uintptr_t place) {
        return val & ~(place - 1);
}

#endif
