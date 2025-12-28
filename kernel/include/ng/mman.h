#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

enum {
	PROT_READ = 1,
	PROT_WRITE = 2,
	PROT_EXEC = 4,
	PROT_NONE = 0,
};

enum {
	MAP_SHARED = 1,
	MAP_PRIVATE = 2,
};

enum {
	MAP_32BIT = 4,
	MAP_ANONYMOUS = 8,
	MAP_FIXED = 16,
	// others
};
