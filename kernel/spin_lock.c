#include <assert.h>
#include <ng/arch/x86_64/interrupt.h>
#include <ng/sync.h>
#include <stdio.h>

void spin_init(spinlock_t *l) {
    atomic_init(&l->lock, 0);
}

int spin_trylock(spinlock_t *spinlock) {
	int expected = 0;
	int desired = 1;
	int success = atomic_compare_exchange_weak_explicit(&spinlock->lock,
		&expected, desired, memory_order_acquire, memory_order_relaxed);
	return success;
}

int spin_lock(spinlock_t *spinlock) {
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
