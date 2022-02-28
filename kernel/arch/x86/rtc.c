#include <basic.h>
#include <stdint.h>
#include <time.h>
#include <x86/cpu.h>
#include <x86/rtc.h>

static int bcd_to_int(uint8_t val) {
    return (val & 0x0F) + ((val & 0xF0) >> 4) * 10;
}

static uint8_t read_rtc(int reg) {
    outb(0x70, reg);
    return bcd_to_int(inb(0x71));
}

struct tm rtc_now(void) {
    int year = read_rtc(9);
    int month = read_rtc(8);
    int day = read_rtc(7);
    int weekday = read_rtc(6);
    int hour = read_rtc(4);
    int minute = read_rtc(2);
    int second = read_rtc(0);

    return (struct tm) {
        .tm_year = year + 100,
        .tm_mon = month - 1,
        .tm_mday = day,
        .tm_wday = weekday - 1,
        .tm_hour = hour,
        .tm_min = minute,
        .tm_sec = second,
    };
}
