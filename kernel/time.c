#include <ng/arch/x86_64/rtc.h>
#include <ng/syscalls.h>
#include <time.h>

sysret sys_btime(time_t *t, struct tm *tm) {
	struct tm now = rtc_now();

	if (t) {
		*t = mktime(&now);
	}

	if (tm) {
		*tm = now;
	}

	return 0;
}

time_t time_now() {
	struct tm now = rtc_now();
	return mktime(&now);
}
