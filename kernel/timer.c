
#include <basic.h>
#include <ng/print.h>
#include <ng/timer.h>
#include <ng/thread.h>
#include <ng/syscall.h>
#include <nc/stdlib.h>
#include <nc/sys/time.h>
#include <nc/assert.h>
#include <stdint.h>

// TODO : arch specific
#include <ng/x86/pit.h>

int seconds(int s) {
        return s * HZ;
}

int milliseconds(int ms) {
        return ms * HZ / 1000;
}

void timer_enable_periodic(int hz) {
        pit_create_periodic(hz);
}

int interrupt_in_ns(long nanoseconds) {
        return pit_create_oneshot(nanoseconds);
}

uint64_t kernel_timer = 0;

enum timer_flags {
        TIMER_RECUR = 0x0001, // How to save the delta_t
};

struct timer_event {
        uint64_t at;
        enum timer_flags flags;
        void (*fn)(void *);
        void *data;
        struct timer_event *next;
        struct timer_event *previous;
};

struct timer_event *timer_head = NULL;

extern void break_here();

void assert_consistency(struct timer_event *t) {
        for (; t; t = t->next) {
                if (t == t->next)  break_here();
                assert(t != t->next);
        }
}

struct timer_event *insert_timer_event(uint64_t delta_t, void (*fn)(void *), void *data) {
        struct timer_event *q = zmalloc(sizeof(struct timer_event));
        // printf("inserting timer event at +%i with %p\n", delta_t, q);
        q->at = kernel_timer + delta_t;
        q->flags = 0;
        q->fn = fn;
        q->data = data;
        q->next = NULL;
        q->previous = NULL;

        if (!timer_head) {
                timer_head = q;
                assert_consistency(timer_head);
                return q;
        }

        if (q->at <= timer_head->at) {
                q->next = timer_head;
                timer_head->previous = q;
                timer_head = q;
                assert_consistency(timer_head);
                return q;
        }

        struct timer_event *te = timer_head;
        for (te=timer_head; te; te=te->next) {
                if (q->at > te->at && te->next)  continue;
                if (!te->next)  break;

                struct timer_event *tmp;
                q->next = te;
                q->previous = te->previous;
                tmp = te->previous;
                te->previous = q;
                if (tmp)  tmp->next = q;
                assert_consistency(timer_head);
                return q;

        }

        te->next = q;
        q->previous = te;
        assert_consistency(timer_head);
        return q;
}

void drop_timer_event(struct timer_event *te) {
        // printf("dropping timer event scheduled for +%i\n",
        //                 te->at - kernel_timer);
        if (te->previous)
                te->previous->next = te->next;
        if (te->next)
                te->next->previous = te->previous;
        free(te);
}

void print_usage_with_timer(void *_) {
        insert_timer_event(seconds(10), print_usage_with_timer, NULL);

        extern int physical_pages_allocated_total;
        extern int physical_pages_freed_total;
        printf("pa:%i", physical_pages_allocated_total);
        printf("pf:%i", physical_pages_freed_total);
}


void timer_callback() {
        kernel_timer += 1;

        while (timer_head && (kernel_timer >= timer_head->at)) {
                struct timer_event *tmp = timer_head;
                timer_head->fn(timer_head->data);
                timer_head = timer_head->next;

                free(tmp);
        }

        switch_thread(SW_TIMEOUT);
}

sysret sys_time() {
        return kernel_timer;
}

