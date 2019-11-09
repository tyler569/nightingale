
#pragma once
#ifndef NG_TIMER_H
#define NG_TIMER_H

#include <basic.h>

struct timer_event;

// timer.c
extern const int HZ;

// seconds to ticks (* HZ)
int seconds(int s);
int milliseconds(int ms);

void timer_enable_periodic(int hz);
int interrupt_in_ns(long nanoseconds);

struct timer_event *insert_timer_event(uint64_t delta_t, void (*)(void *), void *);
void drop_timer_event(struct timer_event *);

void timer_callback(void);

// Temp testing
void print_usage_with_timer();

#endif // NG_TIMER_H

