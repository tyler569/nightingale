
#pragma once
#ifndef NIGHTINGALE_BASIC_H
#define NIGHTINGALE_BASIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ubsan.h"

#define CAT_(x, y) x ## y
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

static_assert(__STDC_HOSTED__ != 1, "Nightingale must not be compiled"
                                    "in a hosted environment");

// convenience macros

#if defined(__x86_64__)
# define X86_64 1
# define I686 0
#elif defined(__i386__) || defined(__i686__)
# define X86_64 0
# define I686 1
#else
# error unsupported architecture
#endif

// basic assumptions

#if X86_64
static_assert(__CHAR_BIT__ == 8, "Bytes must be 8 bits");
static_assert(sizeof(short int) == 2, "Short must be 2 bytes");
static_assert(sizeof(int) == 4, "Int must be 4 bytes");
static_assert(sizeof(long int) == 8, "Long must be 8 bytes");
static_assert(sizeof(void*) == 8, "Pointer must be 8 bytes");
#elif I686
static_assert(__CHAR_BIT__ == 8, "Bytes must be 8 bits");
static_assert(sizeof(short int) == 2, "Short must be 2 bytes");
static_assert(sizeof(int) == 4, "Int must be 4 bytes");
static_assert(sizeof(long int) == 4, "Long must be 4 bytes");
static_assert(sizeof(void*) == 4, "Pointer must be 4 bytes");
#else
#error "unsupported platform"
#endif

typedef signed long ssize_t;
static_assert(sizeof(ssize_t) == sizeof(void*), "long must be pointer width");

// Compiler independant attributes

// You could make the argument that I choose bad names, since '__' is 
// reserved for the compiler, but the odds they use these in the near
// future (esp. since the Linux kernel uses these exact #defines) is
// so low I don't really care.  I like that it's clear that these are
// attributes, and prefer them over the alternatives I know of, (PACKED,
// _packed, or just packed).

#ifdef __GNUC__
# define __packed __attribute__((packed))
# define __noreturn __attribute__((noreturn))
# define __used __attribute__((used))

// maybe switch to this
# define PACKED __packed
# define NORETURN __noreturn
# define USED __used

# ifndef noreturn
#  define noreturn __noreturn
# endif

#else
# error "Need to support non-__GNUC__ attributes.  Edit basic.h for your compiler"
#endif

// GCC stack smasking protection
extern uintptr_t __stack_chk_guard;
void __stack_chk_fail(void);

#define asm __asm__

static inline intptr_t max(intptr_t a, intptr_t b) {
    if (a > b)  return a;
    else        return b;
}

static inline intptr_t min(intptr_t a, intptr_t b) {
    if (a > b)  return b;
    else        return a;
}

static inline uintptr_t round_up(uintptr_t val, uintptr_t place) {
    return (val + place - 1) & ~(place - 1);
}

static inline uintptr_t round_down(uintptr_t val, uintptr_t place) {
    return val & ~(place - 1);
}

#endif

