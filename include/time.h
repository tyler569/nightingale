#pragma once
#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>
#include <stdlib.h>

typedef int64_t time_t;
typedef int64_t clock_t;

#define CLOCKS_PER_SEC 1000
#define HZ CLOCKS_PER_SEC

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;         // years since 1900
    int tm_wday;         // days since Sunday
    int tm_yday;
    int tm_isdst;
};

time_t time(time_t *arg);
clock_t clock(void);
size_t strftime(
    char *str,
    size_t count,
    const char *format,
    const struct tm *time
);

// TODO

struct tm *gmtime(const time_t *timep);
struct tm *gmtime_r(const time_t *timep, struct tm *result);
struct tm *localtime_r(const time_t *timep, struct tm *result);
struct tm *localtime(const time_t *timer);
time_t mktime(struct tm *tm);
double difftime(time_t time1, time_t time0);

#endif // _TIME_H_
