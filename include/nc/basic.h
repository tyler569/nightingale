
#ifndef NG_LIBC_BASIC_H
#define NG_LIBC_BASIC_H

#ifndef _NG

#if defined(__x86_64__)
#define X86 1
#define X86_64 1
#define I686 0
#elif defined(__i686__) || defined(__i386__)
#define X86 1
#define X86_64 0
#define I686 1
#else
#error "unsupported arhitecture"
#endif

#if !defined(__ASSEMBLY__)

#include <stdint.h>

typedef signed long ssize_t;
typedef unsigned long size_t;

#ifndef __cplusplus
#define noreturn _Noreturn
#else
#define noreturn [[noreturn]]
#define restrict
#endif // __cplusplus

#ifdef __GNUC__
#define __unreachable __builtin_unreachable()
#define asm __asm__
#else
#error "Support non-GNUC attributes first"
#endif

static inline intptr_t max(intptr_t a, intptr_t b) {
        return (a) > (b) ? (a) : (b);
}

static inline intptr_t min(intptr_t a, intptr_t b) {
        return (a) > (b) ? (b) : (a);
}

static inline uintptr_t round_up(uintptr_t val, uintptr_t place) {
        return (val + place - 1) & ~(place - 1);
}

static inline uintptr_t round_down(uintptr_t val, uintptr_t place) {
        return val & ~(place - 1);
}

#endif // __ASSEMBLY__

#endif // _NG

#endif
