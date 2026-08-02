#pragma once

#include <sys/cdefs.h>

BEGIN_DECLS

enum {
	PROT_NONE = 0,
	PROT_READ = 1,
	PROT_WRITE = 2,
	PROT_EXEC = 4,

	MAP_SHARED = 1,
	MAP_PRIVATE = 2,
	MAP_32BIT = 4,
	MAP_ANONYMOUS = 8,
};

#define MAP_FAILED (void *)(-1)

#ifndef __kernel__
void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off);
int munmap(void *addr, size_t len);
#endif

END_DECLS
