
#pragma once
#ifndef __BASIC_H__
#define __BASIC_H__

// convenience macros

#if defined(__x86_64__)
#define X86 1
#define X86_64 1
#elif defined(__i386__) || defined(__i686__)
#define X86 1
#define I686 1
#else
#error unsupported architecture
#endif


#ifndef __ASSEMBLER__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define static_assert(A) _Static_assert(A, #A)

#define CAT_(x, y) x##y
#define CAT(x, y) CAT_(x, y)
#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)


#ifdef __kernel__

#include <ng/ubsan.h>

static_assert(__STDC_HOSTED__ != 1);
#define KB (1024)
#define MB (KB * KB)
#define GB (MB * KB)

#endif // __kernel__


// Compiler independant attributes

#ifdef __GNUC__

// legacy
#define _packed    __attribute__((packed))
#define _noreturn  __attribute__((noreturn))
#define _used      __attribute__((used))
#define _align(X)  __attribute__((aligned (X)))

// new
#define __PACKED        __attribute__((packed))
#define __NORETURN      __attribute__((noreturn))
#define __USED          __attribute__((used))
#define __ALIGN(X)      __attribute__((aligned (X)))
#define __NOINLINE      __attribute__((noinline))
#define __RETURNS_TWICE __attribute__((returns_twice))
#define __MUST_USE      __attribute__((warn_unused_result))


#ifndef asm
#define asm __asm__
#endif

#if !defined(noinline) && !defined(IN_GCC)
#define noinline __NOINLINE
#endif

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

#define ARRAY_LEN(a) ((sizeof(a) / sizeof(a[0])))

/*
 * I don't quite know how I feel about these
#define max(a, b) ({ \
        __auto_type __a = (a); \
        __auto_type __b = (b); \
        __a < __b ? __b : __a; \
})
#define min(a, b) ({ \
        __auto_type __a = (a); \
        __auto_type __b = (b); \
        __a < __b ? __a : __b; \
})

#define round_down(v, p) ({ \
        __auto_type __v = (v); \
        __auto_type __p = (p); \
        __v & ~(__p - 1); \
})

#define round_up(v, p) ({ \
        __auto_type __v = (v); \
        __auto_type __p = (p); \
        round_down(__v + __p - 1, __p); \
})
*/

#endif // __ASSEMBLER__

#endif // __BASIC_H__

