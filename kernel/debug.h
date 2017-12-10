
#pragma once
#ifndef NIGHTINGALE_DEBUG_H
#define NIGHTINGALE_DEBUG_H

#include <basic.h>
#include <term/print.h>

#ifndef DEBUG
// Default to not doing debugging, but if DEBUG is defined before
// this file is included, respect that value.
#define DEBUG 0
#endif

/*
 * This is sufficient because the optimizer will remove any
 * if (false) blocks.
 *
 * This also means the debug state is determined per-call,
 * not at include-time, which is the goal.
 */
#define DEBUG_PRINTF(fmt, ...) \
    do { if (DEBUG) printf("[DEBUG] " fmt, ## __VA_ARGS__); } while (0)

#endif

