#include "ng/arch-2.h"
#include "stdatomic.h"

struct spin_lock {
	atomic_int front, back;
};

typedef struct spin_lock spin_lock_t;

void spin_lock(spin_lock_t *lock) {
	int back = atomic_fetch_add_explicit(&lock->back, 1, memory_order_relaxed);

	while (atomic_load_explicit(&lock->front, memory_order_acquire) != back)
		relax_busy_loop();
}

void spin_unlock(spin_lock_t *lock) {
	atomic_fetch_add_explicit(&lock->front, 1, memory_order_release);
}
