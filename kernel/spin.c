#include <assert.h>
#include <ng/sync.h>
#include <ng/x86/interrupt.h>
#include <stdio.h>

int spin_trylock(spinlock_t *spinlock) {
	// assert(irqs_are_disabled());
	int expected = 0;
	int desired = 1;
	int success = atomic_compare_exchange_weak_explicit(&spinlock->lock,
		&expected, desired, memory_order_acquire, memory_order_relaxed);
	return success;
}

int spin_lock(spinlock_t *spinlock) {
	// if (spinlock->lock)
	//     printf("contention! (deadlock?)\n");

	while (!spin_trylock(spinlock)) {
		asm volatile("pause");
	}
	return 1;
}

int spin_unlock(spinlock_t *spinlock) {
	assert(spinlock->lock == 1); // spinlock wasn't taken
	atomic_store_explicit(&spinlock->lock, 0, memory_order_release);
	return 1;
}

int ticket_spin_lock(struct ticket_spinlock *spinlock) {
	int my_ticket
		= atomic_fetch_add_explicit(&spinlock->back, 1, memory_order_acquire);
	while (true) {
		int front
			= atomic_load_explicit(&spinlock->front, memory_order_acquire);
		if (front == my_ticket) {
			break;
		} else {
			asm volatile("pause");
		}
	}
	return 1;
}

int ticket_spin_unlock(struct ticket_spinlock *spinlock) {
	assert(spinlock->front == spinlock->back);
	atomic_store_explicit(
		&spinlock->front, spinlock->front + 1, memory_order_release);
	return 1;
}