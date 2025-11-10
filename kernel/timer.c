#include <assert.h>
#include <ng/fs.h>
#include <ng/irq.h>
#include <ng/spalloc.h>
#include <ng/sync.h>
#include <ng/timer.h>
#include <ng/x86/pit.h>
#include <stdio.h>
#include <sys/time.h>

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
	irq_install(0, timer_handler, nullptr);
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
		list_for_each_safe (&timer_q) {
			struct timer_event *n = container_of(struct timer_event, node, it);
			if (n->at < q->at) {
				list_prepend(it, &q->node);
				added = true;
				break;
			}
		}
		if (!added)
			list_prepend(&timer_q, &q->node);
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

void timer_procfile(struct file *ofd, void *) {
	proc_sprintf(ofd, "The time is: %lu\n", kernel_timer);
	proc_sprintf(ofd, "Pending events:\n");
	spin_lock(&timer_q_lock);
	list_for_each_safe (&timer_q) {
		struct timer_event *t = container_of(struct timer_event, node, it);
		proc_sprintf(ofd, "  %lu (+%lu) \"%s\"\n", t->at, t->at - kernel_timer,
			t->fn_name);
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
		timer_head
			= container_of(struct timer_event, node, list_head(&timer_q));
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
