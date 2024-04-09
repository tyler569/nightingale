#pragma once

#include "sys/cdefs.h"

BEGIN_DECLS

enum event_type {
	EVENT_ALLOC,
	EVENT_FREE,
	EVENT_THREAD_NEW,
	EVENT_THREAD_SWITCH,
	EVENT_THREAD_DIE,
	EVENT_THREAD_REAP,
	EVENT_THREAD_ENQUEUE,
	EVENT_SYSCALL,
	EVENT_SIGNAL,
};

#ifdef __kernel__
void event_log_init(void);
void log_event(enum event_type type, const char *message, ...);
#endif

END_DECLS
