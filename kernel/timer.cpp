#include <assert.h>
#include <ng/fs.h>
#include <ng/irq.h>
#include <ng/spalloc.h>
#include <ng/sync.h>
#include <ng/syscalls.h>
#include <ng/timer.h>
#include <nx/list.h>
#include <stdio.h>
#include <sys/time.h>

#undef insert_timer_event

uint64_t kernel_timer = 0;
static uint64_t last_tsc;
static uint64_t tsc_delta;
struct spalloc timer_pool;
spinlock_t timer_q_lock;

// TODO: constexpr
int seconds(int s) { return s * HZ; }
int milliseconds(int ms) { return ms * HZ / 1000; }

enum timer_flags {
    TIMER_NONE = 0,
};

struct timer_event {
    uint64_t at;
    enum timer_flags flags;

    void (*fn)(void *);
    void *data;

    const char *fn_name;
    nx::list_node node;
};

nx::list<timer_event, &timer_event::node> timer_q;

void timer_init()
{
    sp_init(&timer_pool, struct timer_event);
    irq_install(0, timer_handler, nullptr);
}

void assert_consistency(struct timer_event *t)
{
    assert(t->at < kernel_timer + 10000);
}

struct timer_event *insert_timer_event(
    uint64_t delta_t, void (*fn)(void *), const char *inserter_name, void *data)
{
    auto &q = *(timer_event *)sp_alloc(&timer_pool);
    q.at = kernel_timer + delta_t;
    q.flags = {};
    q.fn = fn;
    q.fn_name = inserter_name;
    q.data = data;

    bool added = false;

    spin_lock(&timer_q_lock);
    if (timer_q.empty()) {
        timer_q.push_back(q);
    } else {
        for (auto &event : timer_q) {
            if (event.at < q.at) {
                timer_q.insert(q, event);
                added = true;
                break;
            }
        }
        if (!added)
            timer_q.push_front(q);
    }
    spin_unlock(&timer_q_lock);

    return &q;
}

void drop_timer_event(struct timer_event *te)
{
    spin_lock(&timer_q_lock);
    timer_q.remove(*te);
    spin_unlock(&timer_q_lock);
    sp_free(&timer_pool, te);
}

extern "C" void timer_procfile(struct file *ofd, void *_)
{
    proc_sprintf(ofd, "The time is: %lu\n", kernel_timer);
    proc_sprintf(ofd, "Pending events:\n");
    spin_lock(&timer_q_lock);
    for (auto &t : timer_q) {
        proc_sprintf(
            ofd, "  %lu (+%lu) \"%s\"\n", t.at, t.at - kernel_timer, t.fn_name);
    }
    spin_unlock(&timer_q_lock);
}

void timer_handler(interrupt_frame *r, void *impl)
{
    kernel_timer += 1;

    uint64_t tsc = rdtsc();
    tsc_delta = tsc - last_tsc;
    last_tsc = tsc;

    spin_lock(&timer_q_lock);
    while (!timer_q.empty()) {
        auto &timer_head = timer_q.front();
        if (timer_head.at > kernel_timer) {
            break;
        }
        timer_q.remove(timer_head);

        spin_unlock(&timer_q_lock);
        timer_head.fn(timer_head.data);
        sp_free(&timer_pool, &timer_head);
        spin_lock(&timer_q_lock);
    }
    spin_unlock(&timer_q_lock);
}

uint64_t timer_now() { return kernel_timer; }

sysret sys_xtime() { return static_cast<sysret>(kernel_timer); }