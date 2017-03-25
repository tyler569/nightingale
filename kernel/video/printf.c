
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "terminal.h"

int printf(const char *fmt, ...) {
    size_t len = strlen(fmt);
    term.write(fmt, len);
    return len;
}

