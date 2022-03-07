#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

clock_t clock(void) {
    return -1;
}

static const char *abbreviated_month[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Nov", "Dec",
};

static const char *full_month[12] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
};

static const char *abbreviated_weekday[7] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
};

static const char *full_weekday[7] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
};

size_t strftime(
    char *restrict str,
    size_t count,
    const char *restrict format,
    const struct tm *restrict time
) {
#define RAW(format) do { \
    if (out_index >= count) return 0; \
    out_index += snprintf(str + out_index, count - out_index, format); \
} while (0)
#define EMIT(format, ...) do { \
    if (out_index >= count) return 0; \
    out_index += snprintf(str + out_index, count - out_index, format, __VA_ARGS__); \
} while (0)

    bool in_format = false;
    size_t out_index = 0;

    for (size_t i = 0; format[i]; i++) {
        char c = format[i];

        // printf("c: (%c), str: \"%s\"\n", c, str);

        if (!in_format && c != '%') {
            str[out_index++] = c;
            continue;
        }

        if (!in_format && c == '%') {
            in_format = true;
            continue;
        }

        switch(c) {
        case '%':
            RAW("%%");
            break;
        case 'n':
            RAW("\n");
            break;
        case 't':
            RAW("\t");
            break;
        case 'Y':
            EMIT("%i", time->tm_year + 1900);
            break;
        case 'y':
            EMIT("%02i", time->tm_year % 100);
            break;
        case 'C':
            EMIT("%02i", ((time->tm_year + 1900) / 100) % 100);
            break;
        case 'G':
        case 'g':
            // unimplemented
            RAW("?");
            break;
        case 'b':
        case 'h':
            EMIT("%s", abbreviated_month[time->tm_wday]);
            break;
        case 'B':
            EMIT("%s", full_month[time->tm_wday]);
            break;
        case 'm':
            EMIT("%02i", time->tm_mon + 1);
            break;
        case 'U':
        case 'W':
        case 'V':
            // unimplemented
            RAW("?");
            break;
        case 'j':
            EMIT("%03i", time->tm_yday + 1);
            break;
        case 'd':
            EMIT("%02i", time->tm_mday);
            break;
        case 'e':
            EMIT("%2i", time->tm_mday);
            break;
        case 'a':
            EMIT("%s", abbreviated_weekday[time->tm_wday]);
            break;
        case 'A':
            EMIT("%s", full_weekday[time->tm_wday]);
            break;
        case 'w':
            EMIT("%i", time->tm_wday);
            break;
        case 'u':
            EMIT("%i", time->tm_wday ? 7 : time->tm_wday);
            break;
        case 'H':
            EMIT("%02i", time->tm_hour);
            break;
        case 'I':
            EMIT("%02i", time->tm_hour % 12 ? 12 : time->tm_hour % 12);
            break;
        case 'M':
            EMIT("%02i", time->tm_min);
            break;
        case 'S':
            EMIT("%02i", time->tm_sec);
            break;
        case 'c':
            EMIT(
                "%s %s %02i %02i:%02i:%02i %i",
                abbreviated_weekday[time->tm_wday],
                abbreviated_month[time->tm_mon],
                time->tm_mday,
                time->tm_hour,
                time->tm_min,
                time->tm_sec,
                time->tm_year + 1900
            );
            break;
        case 'x':
            EMIT(
                "%s %s %02i", 
                abbreviated_weekday[time->tm_wday],
                abbreviated_month[time->tm_mon],
                time->tm_mday
            );
            break;
        case 'T':
            EMIT(
                "%02i:%02i:%02i",
                time->tm_hour,
                time->tm_min,
                time->tm_sec
            );
            break;
        case 'D':
            EMIT(
                "%02i/%02i/%02i",
                time->tm_mon + 1,
                time->tm_mday,
                (time->tm_year + 70) % 100
            );
            break;
        case 'F':
            EMIT(
                "%04i-%02i-%02i",
                time->tm_year + 1900,
                time->tm_mon + 1,
                time->tm_mday
            );
            break;
        case 'r':
        case 'X':
            EMIT(
                "%02i:%02i:%02i %s",
                time->tm_hour % 12 ? 12 : time->tm_hour % 12,
                time->tm_min,
                time->tm_sec,
                time->tm_hour / 12 ? "pm" : "am"
            );
            break;
        case 'R':
            EMIT(
                "%02i:%02i",
                time->tm_hour,
                time->tm_min
            );
            break;
        case 'p':
            EMIT("%s", time->tm_hour / 12 ? "pm" : "am");
            break;
        case 'z':
            RAW("+0000");
            break;
        case 'Z':
            RAW("UTC");
            break;
        default:
            RAW("?");
        }

        in_format = false;
    }

    if (out_index < count)
        str[out_index] = 0;

    return out_index;
}
