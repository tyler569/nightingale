#pragma once
#ifndef NG_TIMER_H
#define NG_TIMER_H

#include <ng/cpu.h>
#include <sys/cdefs.h>

BEGIN_DECLS

#ifdef __kernel__

struct timer_event;

// seconds to ticks (* HZ)
int seconds(int s);
int milliseconds(int ms);

extern uint64_t kernel_timer;

uint64_t timer_now(void);

void timer_enable_periodic(int hz);
void timer_init(void);

struct timer_event *insert_timer_event(uint64_t delta_t, void (*fn)(void *),
	const char *fn_name, void *extra_data);
void drop_timer_event(struct timer_event *);
void timer_handler(interrupt_frame *, void *);

#define insert_timer_event(delta_t, fn, data) \
	insert_timer_event(delta_t, fn, __func__, data)

#endif // __kernel__

END_DECLS

#endif // NG_TIMER_H
