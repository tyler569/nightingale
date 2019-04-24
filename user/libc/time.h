
#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>
#include <stdlib.h>

typedef uint64_t time_t;
typedef uint64_t clock_t;

#define CLOCKS_PER_SEC 100

struct tm {
        int tm_sec;
        int tm_min;
        int tm_hour;
        int tm_mday;
        int tm_mon;
        int tm_year; // years since 1900
        int tm_wday; // days since Sunday
        int tm_yday;
        int tm_isdst;
};

time_t time(time_t *arg);
clock_t clock(void);

size_t strftime(char *str, size_t count, const char *format,
                const struct tm *time);

#endif
