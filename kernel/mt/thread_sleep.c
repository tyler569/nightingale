#include <ng/thread_transition.h>

void unsleep_thread(struct thread *t) {
	t->wait_event = nullptr;
	t->state = TS_RUNNING;
	thread_enqueue(t);
}

void unsleep_thread_callback(void *t) { unsleep_thread(t); }

void sleep_thread(int ms) {
	assert(running_thread->tid != 0);
	int ticks = milliseconds(ms);
	struct timer_event *te
		= insert_timer_event(ticks, unsleep_thread_callback, running_addr());
	running_thread->wait_event = te;
	running_thread->state = TS_SLEEP;
	thread_block();
}

sysret sys_sleepms(int ms) {
	sleep_thread(ms);
	return 0;
}

void thread_timer(void *) {
	insert_timer_event(THREAD_TIME, thread_timer, nullptr);
	thread_yield();
}
