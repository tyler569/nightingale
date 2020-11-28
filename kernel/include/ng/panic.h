#pragma once
#ifndef NG_PANIC_H
#define NG_PANIC_H

#include <basic.h>
#include <assert.h> // temporary
#include <stdio.h>

/*
 * TODO:
 * replace these with something in arch/
 */

#include <ng/cpu.h>
#include <ng/x86/interrupt.h>

noinline void break_point();

#define panic(...)                                                             \
    do {                                                                       \
        break_point();                                                         \
        disable_irqs();                                                        \
        printf("[PANIC] " __VA_ARGS__);                                        \
        halt();                                                                \
        __builtin_unreachable();                                               \
    } while (0)

#define panic_bt(...)                                                          \
    do {                                                                       \
        break_point();                                                         \
        disable_irqs();                                                        \
        printf("[PANIC] " __VA_ARGS__);                                        \
        asm volatile("int $0x82");                                             \
        halt();                                                                \
    } while (0)

#endif // NG_PANIC_H
