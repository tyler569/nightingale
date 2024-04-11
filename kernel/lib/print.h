#pragma once

#include "stream.h"
#include <stdarg.h>
#include <stddef.h>
#include <sys/cdefs.h>

BEGIN_DECLS

int puts(const char *str);

int fprintf(FILE *, const char *format, ...);
int fnprintf(FILE *, size_t n, const char *format, ...);
int vfprintf(FILE *, const char *format, va_list args);
int vfnprintf(FILE *, size_t n, const char *format, va_list args);
int vsnprintf(char *buf, size_t len, const char *format, va_list args);
int vsprintf(char *buf, const char *format, va_list args);
int vprintf(const char *format, va_list args);
int sprintf(char *buf, const char *format, ...) __PRINTF(2, 3);
int snprintf(char *buf, size_t len, const char *format, ...) __PRINTF(3, 4);
int printf(const char *format, ...) __PRINTF(1, 2);

END_DECLS
