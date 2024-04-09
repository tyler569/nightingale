#include <ng/event_log.h>
#include <ng/sync.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <ng/vmm.h>
#include <stdarg.h>
#include <stdio.h>

#define EVENT_LOG_NARGS 8

struct event {
	enum event_type type;
	int timestamp;
	uint64_t args[EVENT_LOG_NARGS];
};

#define EVENT_LOG_SIZE ((1024 * 1024) / sizeof(struct event))
struct event *event_log;
int event_log_index = 0;
int event_log_base = 0;
static mutex_t event_log_lock;

void event_log_init() {
	mutex_init(&event_log_lock);
	event_log = vmm_reserve(EVENT_LOG_SIZE * sizeof(struct event));
}

static bool should_print_event_type(enum event_type type) {
	return ((1 << type) & running_thread->report_events) != 0;
}

void log_event(enum event_type type, const char *message, ...) {
	struct event new_event = { 0 };
	va_list args, printf_args;
	va_start(args, message);
	if (should_print_event_type(type)) {
		va_copy(printf_args, args);
	}

	for (int i = 0; i < EVENT_LOG_NARGS; i++) {
		new_event.args[i] = va_arg(args, uint64_t);
	}
	new_event.type = type;
	new_event.timestamp = timer_now();

	if (should_print_event_type(type)) {
		vprintf(message, printf_args);
	}

	if (mutex_trylock(&event_log_lock)) {
		event_log[event_log_index++] = new_event;
		if (event_log_base == event_log_index) {
			// we've wrapped around and are dropping old events.
			event_log_base += 1;
		}

		// Would these be better as "%=" expressions?
		if (event_log_index >= EVENT_LOG_SIZE) {
			event_log_index = 0;
		}
		if (event_log_base >= EVENT_LOG_SIZE) {
			event_log_base = 0;
		}

		mutex_unlock(&event_log_lock);
	} else {
		// lock was held, couldn't emit event
	}
}
