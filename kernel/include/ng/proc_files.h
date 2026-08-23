#pragma once

#include "sys/cdefs.h"

BEGIN_DECLS

struct file;

struct proc_file_def {
	const char *name;
	void (*fn)(struct file *, void *);
	void *ctx;
};

#define define_proc_file(n, f, c) \
	struct proc_file_def __proc_##f __attribute__((section("proc_files"))) \
	= { .name = (n), .fn = (f), .ctx = (c) };

END_DECLS
