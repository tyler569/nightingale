#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

time_t time(time_t *arg) {
    if (!arg) {
        return 1555381106;
    } else {
        *arg = 1555381106;
        return 0;
    }
}

clock_t clock(void) {
    return -1;
}

size_t strftime(
    char *str,
    size_t count,
    const char *format,
    const struct tm *time
) {
    strcpy(str, "right now");
    // TODO: this should be better
    return strlen(str);
}
