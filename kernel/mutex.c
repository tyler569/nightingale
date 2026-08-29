#include <assert.h>
#include <ng/sync.h>
#include <ng/thread.h>
#include <stdatomic.h>

void mutex_init(mutex_t *m) {
	spin_init(&m->guard);
	m->held = false;
	wq_init(&m->q);
}

void mutex_lock(mutex_t *m) {
	spin_lock(&m->guard);

	while (m->held)
		wq_wait(&m->q, &m->guard);

	m->held = true;

	spin_unlock(&m->guard);
}

void mutex_unlock(mutex_t *m) {
	spin_lock(&m->guard);

	m->held = false;
	wq_wake_one_locked(&m->q);

	spin_unlock(&m->guard);
}

void cv_init(cv_t *v) {
	wq_init(&v->q);
}

void cv_wait(cv_t *v, mutex_t *m) {
	list_append(&v->q.waiting_threads, &running_thread->wq_node);

	mutex_unlock(m);

	sched_block();

	mutex_lock(m);
}

void cv_wake_one_locked(cv_t *v) {
	wq_wake_one_locked(&v->q);
}

void cv_wake_all_locked(cv_t *v) {
	wq_wake_all_locked(&v->q);
}
