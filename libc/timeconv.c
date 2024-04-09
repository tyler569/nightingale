#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SPM 60
#define MPH 60
#define HPD 24
#define SPD (SPM * MPH * HPD)

static const int days_per_month[2][12] = {
	{ 31, 28, 31, 30, 31, 31, 30, 31, 30, 31, 30, 31 },
	{ 31, 29, 31, 30, 31, 31, 30, 31, 30, 31, 30, 31 },
};

__attribute__((pure)) static bool is_leap_year(int year) {
	return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

struct pair {
	int a, b;
};

static struct pair month_day(int days, const int year) {
	int total = 0;
	int i;
	for (i = 0; i < 12 && total <= days; i++) {
		total += days_per_month[is_leap_year(year)][i];
	}
	return (struct pair) { i - 1,
		days - total + days_per_month[is_leap_year(year)][i - 1] };
}

struct tm *gmtime_r(const time_t *_time, struct tm *restrict tm) {
	time_t time = *_time;
	int year;
	for (year = 1970;; year++) {
		int days = is_leap_year(year) ? 366 : 365;
		int seconds = days * SPD;

		if (time - seconds < 0)
			break;

		time -= seconds;
	}
	tm->tm_year = year - 1900;
	tm->tm_yday = time / SPD;
	struct pair m_d = month_day(tm->tm_yday, year);
	tm->tm_mon = m_d.a;
	tm->tm_mday = m_d.b + 1;
	tm->tm_hour = (time % SPD) / 3600;
	tm->tm_min = (time % 3600) / 60;
	tm->tm_sec = time % 60;
	return tm;
}

time_t mktime(struct tm *tm) {
	time_t t = 0;
	int year;
	for (year = 1970; year < tm->tm_year + 1900; year++) {
		int days = is_leap_year(year) ? 366 : 365;
		t += SPD * days;
	}

	int yday = 0;
	for (int i = 0; i < tm->tm_mon; i++) {
		yday += days_per_month[is_leap_year(year)][i];
	}
	yday += tm->tm_mday - 1;
	t += SPD * yday;
	t += 3600 * tm->tm_hour;
	t += 60 * tm->tm_min;
	t += tm->tm_sec;
	return t;
}

#ifndef __kernel__
time_t time(time_t *time) {
	time_t val;
	btime(&val, nullptr);
	if (time)
		*time = val;
	return val;
}
#endif
