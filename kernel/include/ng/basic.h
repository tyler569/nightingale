
#pragma once
#ifndef NG_BASIC_H
#define NG_BASIC_H

#include <ng/ubsan.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define CAT_(x, y) x##y
#define CAT(x, y) CAT_(x, y)

#if __STDC_VERSION__ >= 201112L
#define static_assert _Static_assert
#define noreturn _Noreturn
#else
// Turns out in pure ISO, it's not easy to make a global statement disapear
// since ';' at global scope is not valid.  My solution is to define an
// anonymous global variable, since that can use up the ';' without doing
// anything.  It is `extern` so no storage space is allocated for it.
// I really hope there's a better solution
#define static_assert(cond, err) extern const char CAT(_xx_, __COUNTER__)
#endif

#ifdef __cplusplus
#define noreturn [[noreturn]]
#define restrict
#endif

static_assert(__STDC_HOSTED__ != 1, "Nightingale must not be compiled"
                                    "in a hosted environment");

// convenience macros

#if defined(__x86_64__)
#define X86_64 1
#define I686 0
#define X86 1
#elif defined(__i386__) || defined(__i686__)
#define X86_64 0
#define I686 1
#define X86 1
#else
#error unsupported architecture
#endif

typedef signed long ssize_t;
typedef int pid_t;

// Compiler independant attributes

#ifdef __GNUC__
#define _packed    __attribute__((packed))
#define _noreturn  __attribute__((noreturn))
#define _used      __attribute__((used))
#define _align(X)  __attribute__((aligned (X)))

#else
#error \
    "Need to support non-__GNUC__ attributes.  Edit basic.h for your compiler"
#endif

#if DEBUG_KERNEL
// don't eliminate symbols in debug builds
#define ng_static
#else
#define ng_static static
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

#endif // NG_BASIC_H

