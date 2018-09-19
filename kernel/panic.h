
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

#define panic(...) \
    do { \
        printf("[PANIC] " __VA_ARGS__); \
        vga_flush(); \
        disable_irqs(); \
        halt(); \
        __builtin_unreachable(); \
    } while (0)

#define panic_bt(...) \
    do { \
        printf("[PANIC] " __VA_ARGS__); \
        vga_flush(); \
        asm volatile ("int $0x82"); \
        halt(); \
    } while (0)

#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)

#define assert(cond, ...) \
    do { \
        if (!(cond)) { \
            printf("[ASSERT] " QUOTE(__FILE__) ":" QUOTE(__LINE__) \
                   " '" #cond "' "  __VA_ARGS__); \
            vga_flush(); \
            disable_irqs(); \
            halt(); \
        } \
    } while (0)

#endif

