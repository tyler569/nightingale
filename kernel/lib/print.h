#pragma once

#include "stream.h"
#include <stdarg.h>
#include <stddef.h>
#include <sys/cdefs.h>

BEGIN_DECLS

int fprintf(FILE *, const char *format, ...);
int fnprintf(FILE *, int n, const char *format, ...);
int vfprintf(FILE *, const char *format, va_list args);
int vfnprintf(FILE *, int n, const char *format, va_list args);

END_DECLS
