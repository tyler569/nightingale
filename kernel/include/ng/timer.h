
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
struct timer_event *insert_timer_event(uint64_t delta_t, void (*fn)(void *),
                const char *fn_name, void *extra_data);
void drop_timer_event(struct timer_event *);

void timer_callback(void);

#define insert_timer_event(delta_t, fn, data) \
        insert_timer_event(delta_t, fn, __func__, data)

#ifdef __kernel__

extern uint64_t kernel_timer;

#endif // __kernel__

#endif // NG_TIMER_H

