
#pragma once
#ifndef NIGHTINGALE_PANIC_H
#define NIGHTINGALE_PANIC_H

#include <basic.h>
#include <term/print.h>

#include <cpu/halt.h>
#include <cpu/irq.h>

#define panic(fmt, ...) \
    do { \
        printf("[PANIC] " fmt, ## __VA_ARGS__); \
        disable_irqs(); \
        halt(); \
    } while (0)

#endif
