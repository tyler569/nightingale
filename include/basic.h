
#pragma once
#ifndef __BASIC_H__
#define __BASIC_H__

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


#ifndef __ASSEMBLY__

// #include <ng/ubsan.h>
#include <stddef.h>
#include <stdint.h>

#define CAT_(x, y) x##y
#define CAT(x, y) CAT_(x, y)

#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)

#if __STDC_VERSION__ >= 201112L
#include <stdbool.h>
#include <stdnoreturn.h>
#define static_assert _Static_assert
#endif

#ifdef __cplusplus
#define noreturn [[noreturn]]
#define restrict
#define _Atomic
#endif

#if _NG
static_assert(__STDC_HOSTED__ != 1, "You need a cross compiler");
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

// TODO: remove this
#define ng_static

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

#endif // __ASSEMBLY__

#endif // __BASIC_H__

