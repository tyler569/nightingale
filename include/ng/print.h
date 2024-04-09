#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct writer {
	void *data;
	const struct writer_vtbl *vtbl;
};

struct writer_vtbl {
	void (*write)(struct writer self, const char *data, size_t size);
};

#define WRITER_WRITE(w, d, s) ((w).vtbl->write((w), (d), (s)))

int fprintf(struct writer writer, const char *format, ...);
int fnprintf(struct writer writer, int n, const char *format, ...);
int vfprintf(struct writer writer, const char *format, va_list args);
int vfnprintf(struct writer writer, int n, const char *format, va_list args);

extern struct writer w_stdout;
extern struct writer w_serial;
extern struct writer w_null;

END_DECLS
