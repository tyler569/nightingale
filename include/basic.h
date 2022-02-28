#pragma once
#ifndef _BASIC_H_
#define _BASIC_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// convenience macros
#if defined(__x86_64__)
#define X86 1
#define X86_64 1
#else
#error unsupported architecture
#endif

#define CONCAT_(x, y) x ## y
#define CONCAT(x, y) CONCAT_(x, y)
#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)

#define INCREF(v) ++ ((v)->refcnt)
#define DECREF(v) -- ((v)->refcnt)

#define ARRAY_LEN(A) (sizeof((A)) / sizeof(*(A)))

#define VARIABLE(fragment) CONCAT(fragment, __COUNTER__)
#define BRACKET(before, after) for (int a = (before, 1); a || (after, 0); a = 0)

#define _BENCH(var) \
    for (long a = 1, var = rdtsc(); \
         a || (printf("BENCH %li\n", rdtsc() - var), 0); \
         a = 0)
#define BENCH() _BENCH(VARIABLE(_tsc))

#define PTR_ADD(p, off) (void *)(((char *)p) + off)

static inline bool IS_ERROR(void *R) {
    return (intptr_t)R < 0 && (intptr_t)R > -0x1000;
}

#define TO_ERROR(R) ((void *)(R))
#define ERROR(R) ((intptr_t)(R))

#ifdef __cplusplus
#define BEGIN_DECLS extern "C" {
#define END_DECLS }
#else
#define BEGIN_DECLS
#define END_DECLS
#define static_assert(A) _Static_assert(A, #A)
#endif // __cplusplus

#ifdef __kernel__
static_assert(__STDC_HOSTED__ != 1);
#define KB (1024)
#define MB (KB * KB)
#define GB (MB * KB)
#endif // __kernel__


#ifndef __GNUC__
#error "You'll need to update basic.h to support this non-GNU compiler"
#endif

// Compiler independant attributes
#define __PACKED __attribute__((__packed__))
#define __NORETURN __attribute__((__noreturn__))
#define __USED __attribute__((__used__, __unused__))
#define __MUST_EMIT __USED
#define __MAYBE_UNUSED __attribute__((__unused__))
#define __ALIGN(X) __attribute__((__aligned__(X)))
#define __NOINLINE __attribute__((__noinline__))
#define __RETURNS_TWICE __attribute__((__returns_twice__))
#define __MUST_USE __attribute__((__warn_unused_result__))
#define __WEAK __attribute__((__weak__))

#ifndef asm
#define asm __asm__
#endif

// find a better place for this to live
typedef int clone_fn(void *);

// GCC stack smasking protection
extern uintptr_t __stack_chk_guard;

void __stack_chk_fail(void);

// nice-to-haves
static inline intptr_t max(intptr_t a, intptr_t b) {
    return (a > b) ? a : b;
}

static inline intptr_t min(intptr_t a, intptr_t b) {
    return (a < b) ? a : b;
}

static inline uintptr_t umax(uintptr_t a, uintptr_t b) {
    return (a > b) ? a : b;
}

static inline uintptr_t umin(uintptr_t a, uintptr_t b) {
    return (a < b) ? a : b;
}

static inline uintptr_t round_down(uintptr_t val, uintptr_t place) {
    return val & ~(place - 1);
}

static inline uintptr_t round_up(uintptr_t val, uintptr_t place) {
    return round_down(val + place - 1, place);
}

static inline void delay(int usec) {
    for (volatile int x = 0; x < usec * 10; x++) {
        asm volatile("pause");
    }
}

#endif // _BASIC_H_
