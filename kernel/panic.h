
#pragma once
#ifndef NIGHTINGALE_PANIC_H
#define NIGHTINGALE_PANIC_H

#include <basic.h>
#include <term/print.h>

#define panic(fmt, ...) \
    do { \
        printf("\n[PANIC] " fmt, ## __VA_ARGS__); \
        __asm__("int $1"); \
    } while (0)

#endif
