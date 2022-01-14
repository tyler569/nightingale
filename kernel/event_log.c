#include <basic.h>
#include <stdarg.h>
#include <string.h>
#include <ng/event_log.h>
#include <ng/sync.h>
#include <ng/ringbuf.h>
#include <ng/timer.h>
#include <ng/vmm.h>

struct event {
    enum event_type type;
    int message_length;
    uint64_t timestamp;
    uint64_t args[8];
    char message[];
};

void *event_log;
struct ringbuf event_log_ring;
mutex_t event_log_lock = MUTEX_INIT(event_log_lock);
#define EVENT_LOG_SIZE (1 * MB)

int bytes_written = 0;

void event_log_init() {
    event_log = vmm_reserve(EVENT_LOG_SIZE);
    emplace_ring_with_buffer(&event_log_ring, EVENT_LOG_SIZE, event_log);
}

void log_event(enum event_type type, const char *message, ...) {
    struct event new_event = {0};
    va_list args;
    va_start(args, message);
    for (int i = 0; i < 8; i++) {
        new_event.args[i] = va_arg(args, uint64_t);
    }
    new_event.type = type;
    size_t message_len = round_up(strlen(message), 8);
    new_event.message_length = message_len;
    new_event.timestamp = timer_now();

    mutex_lock(&event_log_lock);

    ring_write(&event_log_ring, &new_event, sizeof(new_event));
    bytes_written += sizeof(new_event);
    // TODO: pad with 0s, not by spilling off the end
    ring_write(&event_log_ring, message, message_len);
    bytes_written += message_len;

    mutex_unlock(&event_log_lock);
}
