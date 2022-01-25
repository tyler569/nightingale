#include <basic.h>
#include <assert.h>
#include <ng/fs.h>
#include <ng/irq.h>
#include <ng/spalloc.h>
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
struct spalloc timer_pool;
list timer_q = LIST_INIT(timer_q);
spinlock_t timer_q_lock;

int seconds(int s) {
    return s * HZ;
}

int milliseconds(int ms) {
    return ms * HZ / 1000;
}

void timer_enable_periodic(int hz) {
    // spin_init(&timer_q_lock);
    pit_create_periodic(hz);
    printf("timer: ticking at %i HZ\n", hz);
}

enum timer_flags {
    TIMER_NONE = 0,
};

struct timer_event {
    uint64_t at;
    enum timer_flags flags;

    void (*fn)(void *);
    void *data;

    const char *fn_name;
    list_node node;
};

void timer_init() {
    sp_init(&timer_pool, struct timer_event);
    irq_install(0, timer_handler, NULL);
}

void assert_consistency(struct timer_event *t) {
    assert(t->at < kernel_timer + 10000);
}

struct timer_event *insert_timer_event(uint64_t delta_t, void (*fn)(void *),
                                       const char *inserter_name, void *data) {
    struct timer_event *q = sp_alloc(&timer_pool);
    q->at = kernel_timer + delta_t;
    q->flags = 0;
    q->fn = fn;
    q->fn_name = inserter_name;
    q->data = data;

    bool added = false;

    spin_lock(&timer_q_lock);
    if (list_empty(&timer_q)) {
        list_append(&timer_q, &q->node);
    } else {
        list_for_each(struct timer_event, n, &timer_q, node) {
            if (n->at < q->at) {
                list_prepend(&n->node, &q->node);
                added = true;
                break;
            }
        }
        if (!added) list_prepend(&timer_q, &q->node);
    }
    spin_unlock(&timer_q_lock);

    return q;
}

void drop_timer_event(struct timer_event *te) {
    spin_lock(&timer_q_lock);
    list_remove(&te->node);
    spin_unlock(&timer_q_lock);
    sp_free(&timer_pool, te);
}

void timer_procfile(struct open_file *ofd, void *_) {
    proc_sprintf(ofd, "The time is: %llu\n", kernel_timer);
    proc_sprintf(ofd, "Pending events:\n");
    spin_lock(&timer_q_lock);
    list_for_each(struct timer_event, t, &timer_q, node) {
        proc_sprintf(ofd, "  %llu (+%llu) \"%s\"\n", t->at,
                     t->at - kernel_timer, t->fn_name);
    }
    spin_unlock(&timer_q_lock);
}

void timer_handler(interrupt_frame *r, void *impl) {
    kernel_timer += 1;

    long long tsc = rdtsc();
    tsc_delta = tsc - last_tsc;
    last_tsc = tsc;

    spin_lock(&timer_q_lock);
    while (!list_empty(&timer_q)) {
        struct timer_event *timer_head;
        timer_head = list_head(struct timer_event, node, &timer_q);
        if (timer_head->at > kernel_timer) {
            break;
        }
        list_remove(&timer_head->node);

        spin_unlock(&timer_q_lock);
        timer_head->fn(timer_head->data);
        sp_free(&timer_pool, timer_head);
        spin_lock(&timer_q_lock);
    }
    spin_unlock(&timer_q_lock);
}

uint64_t timer_now() {
    return kernel_timer;
}

sysret sys_xtime() {
    return kernel_timer;
}
