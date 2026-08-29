#include <list.h>
#include <ng/sync.h>
#include <ng/thread.h>

void wq_init(struct wait_queue *q) {
	list_init(&q->waiting_threads);
}

void wq_wait(struct wait_queue *q, spinlock_t *l) {
	list_append(&q->waiting_threads, &running_thread->wq_node);

	spin_unlock(l);

	sched_block();

	spin_lock(l);
}

void wq_wake_one_locked(struct wait_queue *q) {
	if (list_empty(&q->waiting_threads))
		return;

	list_node *it = list_pop_front(&q->waiting_threads);
	struct thread *th = container_of(struct thread, wq_node, it);
	sched_wake(th);
}

void wq_wake_all_locked(struct wait_queue *q) {
	while (!list_empty(&q->waiting_threads)) {
		list_node *it = list_pop_front(&q->waiting_threads);
		struct thread *th = container_of(struct thread, wq_node, it);

		sched_wake(th);
	}
}
