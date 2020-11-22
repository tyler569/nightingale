#pragma once
#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_

#include <time.h>

typedef long suseconds_t;

struct timeval {
    time_t tv_sec;
    suseconds_t tv_usec;
};

int gettimeofday(struct timeval *, void *);
// int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
// int utimes(const char *, const struct timeval[2]);

#endif // _SYS_TIME_H_
