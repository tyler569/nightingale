
#pragma once
#ifndef NIGHTINGALE_PANIC_H
#define NIGHTINGALE_PANIC_H

#include <basic.h>
#include <print.h>

extern int backtrace_from_here(int frames);

/*
 * TODO:
 * replace these with something in arch/
 */
#include <arch/x86/halt.h>
#include <arch/x86/interrupt.h>
#include <arch/x86/vga.h>

#define panic(fmt, ...) \
    do { \
        printf("[PANIC] " fmt, ## __VA_ARGS__); \
        vga_flush(); \
        disable_irqs(); \
        halt(); \
        __builtin_unreachable(); \
    } while (0)

#define panic_bt(fmt, ...) \
    do { \
        printf("[PANIC] " fmt, ## __VA_ARGS__); \
        vga_flush(); \
        asm volatile ("int $0x82"); \
        halt(); \
    } while (0)

#define QUOTE(x) #x

#define assert(cond, fmt, ...) \
    do { \
        if (!(cond)) { \
            printf("[ASSERT] %s:%i '" #cond "' " fmt, \
                    __FILE__, __LINE__, ## __VA_ARGS__); \
            vga_flush(); \
            disable_irqs(); \
            halt(); \
        } \
    } while (0)

#endif

