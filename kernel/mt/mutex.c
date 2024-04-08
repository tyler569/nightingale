#include <assert.h>
#include <ng/mutex.h>
#include <ng/thread.h>
#include <stdatomic.h>

atomic_int next_mutex_id = 1;

void mutex_init(mutex_t *mutex) {
	int id = atomic_fetch_add_explicit(&next_mutex_id, 1, memory_order_relaxed);
	mutex->lock = 0;
	mutex->ticket = 0;
	mutex->id = id;
}

mutex_t make_mutex() {
	mutex_t tmp;
	mutex_init(&tmp);
	return tmp;
}

bool mutex_trylock(mutex_t *mutex) {
	assert(mutex->id != 0);

	int expected = 0;
	int desired = -1;
	// fast path
	if (mutex->lock != expected)
		return false;
	return atomic_compare_exchange_weak_explicit(&mutex->lock, &expected,
		desired, memory_order_acquire, memory_order_relaxed);
}

// Alternate form to operate as a condition variable, seeing if we can use
// the same guts for both.
void wait_on_mutex_cv(mutex_t *condvar, mutex_t *mutex) {
	assert(condvar->id != 0);

	if (mutex)
		mutex_unlock(mutex);

	running_thread->state = TS_BLOCKED;
	running_thread->awaiting_mutex = condvar->id;
	int ticket
		= atomic_fetch_add_explicit(&condvar->ticket, 1, memory_order_relaxed);
	running_thread->awaiting_deli_ticket = ticket;
	atomic_fetch_add_explicit(&condvar->waiting, 1, memory_order_relaxed);
	thread_block();

	if (mutex)
		mutex_lock(mutex);
}

void wait_on_mutex(mutex_t *mutex) { wait_on_mutex_cv(mutex, NULL); }

int mutex_lock(mutex_t *mutex) {
	while (!mutex_trylock(mutex)) {
		wait_on_mutex(mutex);
	}
	return true;
}

void wake_awaiting_thread(mutex_t *mutex) {
	assert(mutex->id != 0);

	// fast path
	if (!mutex->waiting)
		return;
	int n_awaiting = 0;
	int best_deli_ticket = INT_MAX;
	struct thread *winning_thread = NULL;
	list_for_each_safe (&all_threads) {
		struct thread *th = container_of(struct thread, all_threads, it);
		if (th->state != TS_BLOCKED)
			continue;
		if (th->awaiting_mutex != mutex->id)
			continue;

		n_awaiting += 1;
		if (best_deli_ticket > th->awaiting_deli_ticket) {
			best_deli_ticket = th->awaiting_deli_ticket;
			winning_thread = th;
		}
	}
	mutex->waiting = n_awaiting; // racey
	if (!winning_thread)
		return;
	winning_thread->state = TS_RUNNING;
	winning_thread->awaiting_mutex = 0;
	thread_enqueue(winning_thread);
}

// This doesn't make a lot of sense for a mutex, but I'm trying to see if I
// can use the same guts for a wait queue, and this does make a lot of sense
// there.
void wake_all_awaiting_threads(mutex_t *mutex) {
	// fast path
	if (!mutex->waiting)
		return;
	list_for_each_safe (&all_threads) {
		struct thread *th = container_of(struct thread, all_threads, it);
		if (th->state != TS_BLOCKED)
			continue;
		if (th->awaiting_mutex != mutex->id)
			continue;

		th->state = TS_RUNNING;
		th->awaiting_mutex = 0;
		thread_enqueue(th);
	}
	mutex->waiting = 0;
}

int mutex_unlock(mutex_t *mutex) {
	atomic_store_explicit(&mutex->lock, 0, memory_order_release);
	wake_awaiting_thread(mutex);
	return true;
}
