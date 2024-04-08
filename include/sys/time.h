#pragma once

#include <sys/cdefs.h>
#include <time.h>

BEGIN_DECLS

typedef long suseconds_t;

struct timeval {
	time_t tv_sec;
	suseconds_t tv_usec;
};

int gettimeofday(struct timeval *, void *);
// int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
// int utimes(const char *, const struct timeval[2]);

END_DECLS

