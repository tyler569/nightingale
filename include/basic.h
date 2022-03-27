#pragma once
#ifndef _BASIC_H_
#define _BASIC_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __kernel__

// convenience macros
#if defined(__x86_64__)
#define X86 1
#define X86_64 1
#else
#error unsupported architecture
#endif

_Static_assert(__STDC_HOSTED__ != 1, "Kernel cannot be built hosted");
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
#define __PRINTF(index, firstarg) \
    __attribute__((__format__(__printf__, index, firstarg)))
#define __MALLOC(...) __attribute__((__malloc__, __alloc_size__(__VA_ARGS__)))

// GCC stack smasking protection
extern uintptr_t __stack_chk_guard;
void __stack_chk_fail(void);

#ifdef __cplusplus
#define BEGIN_DECLS extern "C" {
#define END_DECLS }
#else
#define BEGIN_DECLS
#define END_DECLS
#define static_assert(A) _Static_assert(A, #A)
#endif // __cplusplus

#if _NG_SOURCE > 0

// find a better place for this to live
typedef int clone_fn(void *);

#define CONCAT_(x, y) x##y
#define CONCAT(x, y) CONCAT_(x, y)
#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)

#define ARRAY_LEN(A) (sizeof((A)) / sizeof(*(A)))

#define PTR_ADD(p, off) (void *)(((char *)p) + off)

#define IS_ERROR(R) \
    ({ \
        __auto_type _r = (R); \
        (intptr_t) _r > -0x1000 && (intptr_t)_r < 0; \
    })

#define TO_ERROR(R) ((void *)(intptr_t)(R))
#define ERROR(R) ((intptr_t)(R))

#define max(A, B) \
    ({ \
        __auto_type _a = (A); \
        __auto_type _b = (B); \
        _a < _b ? _b : _a; \
    })

#define min(A, B) \
    ({ \
        __auto_type _a = (A); \
        __auto_type _b = (B); \
        _a < _b ? _a : _b; \
    })

#define umax max
#define umin min

#define round_down(val, place) \
    ({ \
        __auto_type _v = (val); \
        __auto_type _p = (place); \
        _v & ~(_p - 1); \
    })

#define round_up(val, place) \
    ({ \
        __auto_type _v = (val); \
        __auto_type _p = (place); \
        (_v + _p - 1) & ~(_p - 1); \
    })

#define delay(usec) \
    do { \
        for (volatile int x = 0; x < (usec)*10; x++) \
            asm volatile("pause"); \
    } while (0)

#endif

#endif // _BASIC_H_
