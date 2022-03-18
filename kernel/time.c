#include <basic.h>
#include <ng/syscalls.h>
#include <time.h>
#include <x86/rtc.h>

sysret sys_btime(time_t *t, struct tm *tm)
{
    struct tm now = rtc_now();

    if (t) {
        *t = mktime(&now);
    }

    if (tm) {
        *tm = now;
    }

    return 0;
}
