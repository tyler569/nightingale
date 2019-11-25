
#include <basic.h>
#include <ng/print.h>
#include <ng/spalloc.h>
#include <ng/syscall.h>
#include <ng/timer.h>
#include <ng/thread.h>
#include <ng/fs.h>
#include <nc/stdlib.h>
#include <nc/sys/time.h>
#include <nc/assert.h>
#include <stdint.h>

// TODO : arch specific
#include <ng/x86/pit.h>

#undef insert_timer_event

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
        const char *fn_name;
        void *data;
        struct timer_event *next;
        struct timer_event *previous;
};

struct spalloc timer_event_allocator;

void init_timer_events() {
        sp_init(&timer_event_allocator, struct timer_event);
}

struct timer_event *timer_head = NULL;

void assert_consistency(struct timer_event *t) {
        for (; t; t = t->next) {
                assert(t != t->next);
        }
}

struct timer_event *insert_timer_event(uint64_t delta_t, void (*fn)(void *),
                        const char *inserter_name, void *data) {
        struct timer_event *q = sp_alloc(&timer_event_allocator);
        // printf("inserting timer event at +%i with %p\n", delta_t, q);
        q->at = kernel_timer + delta_t;
        q->flags = 0;
        q->fn = fn;
        q->fn_name = inserter_name;
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
        sp_free(&timer_event_allocator, te);
}

int timer_procfile(struct open_file *ofd) {
        ofd->buffer = malloc(4096);
        int x = 0;
        x += sprintf(ofd->buffer + x, "The time is: %llu\n", kernel_timer);
        x += sprintf(ofd->buffer + x, "Pending events:\n");

        for (struct timer_event *t = timer_head; t; t = t->next) {
                x += sprintf(ofd->buffer + x, "  %llu (%+llu) \"%s\"\n",
                                t->at, t->at - kernel_timer, t->fn_name);
        }
        ofd->length = x;
        return 0;
}

void timer_callback() {
        kernel_timer += 1;

        while (timer_head && (kernel_timer >= timer_head->at)) {
                struct timer_event *te = timer_head;
                timer_head = timer_head->next;

                // printf("running a timer function (t: %i)\n", kernel_timer);
                // printf("it was scheduled for %i, and is at %p\n", te->at, te);

                te->fn(te->data);
                sp_free(&timer_event_allocator, te);
        }
}

sysret sys_time() {
        return kernel_timer;
}

