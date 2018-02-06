
#pragma once
#ifndef NIGHTINGALE_DEBUG_H
#define NIGHTINGALE_DEBUG_H

#include <basic.h>
#include <print.h>
#include <terminal.h>

#ifdef DEBUG

// only debug if DEBUG defined before import

#define DEBUG_PRINTF(fmt, ...) \
    do { printf("[DEBUG] " fmt, ## __VA_ARGS__); } while (0)

#else

#define DEBUG_PRINTF(...)

#endif

#define WARN_PRINTF(fmt, ...) \
    do { printf("[WARN!] " fmt, ## __VA_ARGS__); } while (0)

#endif

