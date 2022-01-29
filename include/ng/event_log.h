#pragma once
#ifndef NG_EVENT_LOG
#define NG_EVENT_LOG

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

void event_log_init(void);
void log_event(enum event_type type, const char *message, ...);

#endif
