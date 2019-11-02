
#pragma once
#ifndef NG_PANIC_H
#define NG_PANIC_H

#include <ng/basic.h>
#include <ng/print.h>
/*
 * TODO:
 * replace these with something in arch/
 */
#include <ng/x86/halt.h>
#include <ng/x86/interrupt.h>

#define panic(...) \
        do { \
                disable_irqs(); \
                printf("[PANIC] " __VA_ARGS__); \
                halt(); \
                __builtin_unreachable(); \
        } while (0)

#define panic_bt(...) \
        do { \
                disable_irqs(); \
                printf("[PANIC] " __VA_ARGS__); \
                asm volatile("int $0x82"); \
                halt(); \
        } while (0)

#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)

#define assert(cond, ...) \
        do { \
                if (!(cond)) { \
                        disable_irqs(); \
                        printf("[ASSERT] " QUOTE(__FILE__) ":" QUOTE( \
                            __LINE__) " '" #cond "' " __VA_ARGS__); \
                        asm volatile("int $0x82"); \
                        halt(); \
                } \
        } while (0)

#endif // NG_PANIC_H

