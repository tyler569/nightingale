
#pragma once
#ifndef __BASIC_H__
#define __BASIC_H__

// convenience macros

#if defined(__x86_64__)
#define X86_64 1
#define X86 1
#elif defined(__i386__) || defined(__i686__)
#define I686 1
#define X86 1
#else
#error unsupported architecture
#endif


#ifndef __ASSEMBLER__

#include <stddef.h>
#include <stdint.h>

#define CAT_(x, y) x##y
#define CAT(x, y) CAT_(x, y)

#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)

#if __STDC_VERSION__ >= 201112L

#include <stdbool.h>
#include <stdnoreturn.h>
#include <stdatomic.h>
#define static_assert _Static_assert

#endif // __STDC_VERSION__ > 201112L

#define KB (1024)
#define MB (KB * KB)
#define GB (MB * KB)

#if _NG

#include <ng/ubsan.h>

static_assert(__STDC_HOSTED__ != 1, "You need a cross compiler");

#define _NC_LOCATION_MALLOC 1

#endif // _NG

typedef intptr_t ssize_t;
typedef int pid_t;

// Compiler independant attributes

#ifdef __GNUC__

#define _packed    __attribute__((packed))
#define _noreturn  __attribute__((noreturn))
#define _used      __attribute__((used))
#define _align(X)  __attribute__((aligned (X)))
#define noinline   __attribute__((noinline))

#define asm __asm__

// GCC stack smasking protection
extern uintptr_t __stack_chk_guard;
void __stack_chk_fail(void);

#else

#error "Not __GNUC__ -- Edit basic.h for your compiler"

#endif // __GNUC__

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

#endif // __ASSEMBLER__

#endif // __BASIC_H__

