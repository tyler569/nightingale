#include <ng/syscalls.h>
#include <ng/x86/rtc.h>
#include <time.h>

sysret sys_btime(time_t *t, tm *m)
{
    tm now = rtc_now();

    if (t) {
        *t = mktime(&now);
    }

    if (m) {
        *m = now;
    }

    return 0;
}

time_t time_now(void)
{
    tm now = rtc_now();
    return mktime(&now);
}
