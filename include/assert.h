#pragma once
#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/cdefs.h>

#ifdef __kernel__
#include <ng/panic.h>
#define __assert_exit(x) panic_bt("assert")
#else
#define __assert_exit(x) exit(x)
#endif

#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)

#ifndef NDEBUG

#define assert(assertion) \
    do { \
        if (!(assertion)) { \
            printf("[ASSERT] '" #assertion "' @ " __FILE__ \
                   ":" QUOTE(__LINE__) "\n"); \
            __assert_exit(1); \
            __builtin_unreachable(); \
        } \
    } while (0)

#define _UNREACHABLE() assert("not reachable" && 0)

#else // NDEBUG
#define assert(...)
#define _UNREACHABLE()
#endif // NDEBUG

#endif // _ASSERT_H_
