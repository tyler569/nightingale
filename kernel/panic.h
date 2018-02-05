
#pragma once
#ifndef NIGHTINGALE_PANIC_H
#define NIGHTINGALE_PANIC_H

#include <basic.h>
#include <term/print.h>

#include <cpu/halt.h>
#include <cpu/interrupt.h>

#define panic(fmt, ...) \
    do { \
        printf("\n[PANIC] " fmt "\n", ## __VA_ARGS__); \
        disable_irqs(); \
        halt(); \
    } while (0)

#define assert(cond, fmt, ...) \
    do { \
        if (!(cond)) { \
            printf("\n[ASSERT] %s:%i " fmt "\n", __FILE__, __LINE__, ## __VA_ARGS__); \
            disable_irqs(); \
            halt(); \
        } \
    } while (0)

#endif
