
#pragma once
#ifndef NG_TIMER_H
#define NG_TIMER_H

#include <basic.h>

struct timer_event;

// seconds to ticks (* HZ)
int seconds(int s);
int milliseconds(int ms);

void timer_enable_periodic(int hz);
int interrupt_in_ns(long nanoseconds);

void init_timer_events(void);
struct timer_event *insert_timer_event(uint64_t delta_t, void (*)(void *), void *);
void drop_timer_event(struct timer_event *);

void timer_callback(void);

#endif // NG_TIMER_H

