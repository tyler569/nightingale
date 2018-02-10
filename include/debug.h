
#pragma once
#ifndef NIGHTINGALE_DEBUG_H
#define NIGHTINGALE_DEBUG_H

#include <basic.h>
#include <print.h>

#define DEBUG3 "\x01"
#define DEBUG2 "\x02"
#define DEBUG_ "\x03" // RENAME ME when #define DEBUG is gone
#define WARN   "\x04"
#define ERROR  "\x05"
#define CRIT   "\x06"

static const char *debug_types[] = {
    NULL,
    "DEBUG3",
    "DEBUG2",
    "DEBUG",
    "WARN",
    "ERROR",
    "CRIT"
};

static /* static for now, move to a .c later probably */
usize debug_printf(const char *fmt, ...) {
    // va_magic
    switch (fmt[0]) {
    case 1: //DEBUG3: //DEBUG3[0]: ?
    case 2: //DEBUG2:
    case 3: //DEBUG:
    case 4: //WARN:
    case 5: //ERROR:
    case 6: //CRIT:
    default:
        break;
    }
    return printf("not implemented yet\n");
}

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

