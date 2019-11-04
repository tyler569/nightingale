
#pragma once
#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <basic.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _NG
#include <ng/panic.h>
#define __assert_exit(x) panic_bt("assert")
#else
#define __assert_exit(x) exit(x)
#endif // _NG

#ifndef NDEBUG

#define assert(assertion) do { \
    if (!(assertion)) { \
        printf("[ASSERT] '" #assertion "' @ " __FILE__ ":" \
                QUOTE(__LINE__) "\n"); \
        __assert_exit(1); \
        __builtin_unreachable(); \
    } \
    } while (0)

#else // NDEBUG

#define assert(...)

#endif // NDEBUG

#endif // _ASSERT_H_

