
#pragma once
#ifndef NIGHTINGALE_PANIC_H
#define NIGHTINGALE_PANIC_H

#include <basic.h>
#include <print.h>

#include <cpu/halt.h>
#include <cpu/interrupt.h>

#define panic(fmt, ...) \
    do { \
        printf("\n[PANIC] " fmt "\n", ## __VA_ARGS__); \
        vga_flush(); \
        disable_irqs(); \
        halt(); \
    } while (0)

#define QUOTE(x) #x

#define assert(cond, fmt, ...) \
    do { \
        if (!(cond)) { \
            printf("\n[ASSERT] %s:%i '" #cond "' " fmt "\n", \
                    __FILE__, __LINE__,## __VA_ARGS__); \
            vga_flush(); \
            disable_irqs(); \
            halt(); \
        } \
    } while (0)

#endif
