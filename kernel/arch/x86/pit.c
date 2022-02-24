// #define DEBUG
#include <basic.h>
#include <ng/cpu.h>
#include <ng/debug.h>
#include <stdbool.h>
#include <stdio.h>
#include <x86/pit.h>

#define PIT_CH0 0x40
#define PIT_CH1 0x41
#define PIT_CH2 0x42
#define PIT_CMD 0x43

#define CHANNEL_0 0x00
#define CHANNEL_1 0x40
#define CHANNEL_2 0x80

#define ACCESS_HILO 0x30

#define MODE_0 0x00 // terminal count
#define MODE_1 0x02 // hw oneshot
#define MODE_2 0x04 // rate generator
#define MODE_3 0x06 // square wave
#define MODE_4 0x08 // sw strobe
#define MODE_5 0x0A // hw strobe

int pit_create_periodic(int hz) {
    int divisor = 1193182 / hz;

    // 0 represents 65536 and is the largest possible divisor
    // giving 18.2Hz
    if (divisor > 65535)
        divisor = 0;

    outb(PIT_CMD, CHANNEL_0 | ACCESS_HILO | MODE_3);

    outb(PIT_CH0, divisor & 0xFF);     /* Set low byte of divisor */
    outb(PIT_CH0, divisor >> 8);     /* Set high byte of divisor */

    return 0;
}

bool ignore_timer_interrupt = false;

int pit_create_oneshot(int microseconds) {
    DEBUG_PRINTF("creating oneshot with %ius\n", microseconds);

    int hz = 1000000 / microseconds;
    int divisor = 1193182 / hz;

    // 0 represents 65536 and is the largest possible divisor
    // giving 18.2Hz
    if (divisor > 65535) {
        printf("pit: warning, time > 55ms clamped\n");
        divisor = 0;
    }

    outb(PIT_CMD, CHANNEL_0 | ACCESS_HILO | MODE_4);

    outb(PIT_CH0, divisor & 0xFF);     /* Set low byte of divisor */
    outb(PIT_CH0, divisor >> 8);     /* Set high byte of divisor */

    ignore_timer_interrupt = false;

    return 0;
}

int pit_ignore(void) {
    ignore_timer_interrupt = true;
    return 0;
}
