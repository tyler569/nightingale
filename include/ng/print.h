#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct writer {
	void (*write)(struct writer *self, const char *data, size_t size);
};

size_t fprintf(struct writer *writer, const char *format, ...);
size_t fnprintf(struct writer *writer, size_t n, const char *format, ...);
size_t vfprintf(struct writer *writer, const char *format, va_list args);
size_t vfnprintf(
	struct writer *writer, size_t n, const char *format, va_list args);

struct writer *stdout;
struct writer *serial;
struct writer *null;

END_DECLS