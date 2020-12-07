#include <basic.h>
#include <assert.h>
#include <ng/fs.h>
#include <ng/irq.h>
#include <ng/sync.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <x86/pit.h>

#undef insert_timer_event

uint64_t kernel_timer = 0;
static long long last_tsc;
static long long tsc_delta;
list timer_q = LIST_INIT(timer_q);
struct mutex timer_q_lock = MTX_INIT(timer_q_lock);

int seconds(int s) {
    return s * HZ;
}

int milliseconds(int ms) {
    return ms * HZ / 1000;
}

void timer_enable_periodic(int hz) {
    pit_create_periodic(hz);
    printf("timer: ticking at %i HZ\n", hz);
}

enum timer_flags {
    TIMER_NONE,
};

struct timer_event {
    uint64_t at;
    enum timer_flags flags;

    void (*fn)(void *);

    const char *fn_name;
    void *data;
    list_node node;
};

void init_timer() {
    irq_install(0, timer_handler, NULL);
}

void assert_consistency(struct timer_event *t) {
    assert(t->at < kernel_timer + 10000);
}

struct timer_event *insert_timer_event(uint64_t delta_t, void (*fn)(void *),
                                       const char *inserter_name, void *data) {
    struct timer_event *q = zmalloc(sizeof(struct timer_event));
    q->at = kernel_timer + delta_t;
    q->flags = 0;
    q->fn = fn;
    q->fn_name = inserter_name;
    q->data = data;

    with_lock(&timer_q_lock) {
        if (list_empty(&timer_q)) {
            list_append(&timer_q, &q->node);
        } else {
            list_for_each(struct timer_event, n, &timer_q, node) {
                if (n->at < q->at) {
                    list_prepend(&n->node, &q->node);
                    break;
                }
            }
            list_append(&timer_q, &q->node);
        }
    }

    return q;
}

void drop_timer_event(struct timer_event *te) {
    with_lock(&timer_q_lock) {
        list_remove(&te->node);
    }
    free(te);
}

void timer_procfile(struct open_file *ofd, void *_) {
    proc_sprintf(ofd, "The time is: %llu\n", kernel_timer);
    proc_sprintf(ofd, "Pending events:\n");
    list_for_each(struct timer_event, t, &timer_q, node) {
        proc_sprintf(ofd, "  %llu (+%llu) \"%s\"\n", t->at,
                     t->at - kernel_timer, t->fn_name);
    }
}

void timer_handler(interrupt_frame *r, void *impl) {
    kernel_timer += 1;

    long long tsc = rdtsc();
    tsc_delta = tsc - last_tsc;
    last_tsc = tsc;

    while (!list_empty(&timer_q)) {
        struct timer_event *timer_head;
        with_lock(&timer_q_lock) {
            timer_head = list_head(struct timer_event, node, &timer_q);
        }
        if (timer_head->at > kernel_timer) break;
        with_lock(&timer_q_lock) {
            list_remove(&timer_head->node);
        }

        // printf("running a timer function (t: %i)\n", kernel_timer);
        // printf("it was scheduled for %i, and is at %p\n", te->at, te);

        if ((uintptr_t)timer_head->fn < 0xF000000000000000) {
            printf("%p\n", timer_head->fn);
            panic();
        } else {
            timer_head->fn(timer_head->data);
        }
        free(timer_head);
    }

    // assert_consistency(timer_head);
}

sysret sys_xtime() {
    return kernel_timer;
}
