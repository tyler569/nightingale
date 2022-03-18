#include <basic.h>
#include <ng/panic.h>
#include <ng/sync.h>
#include <stdio.h>

int spin_trylock(spinlock_t *spinlock)
{
    assert(irqs_are_disabled());
    int expected = 0;
    int desired = 1;
    int success = atomic_compare_exchange_weak_explicit(&spinlock->lock,
        &expected, desired, memory_order_acquire, memory_order_relaxed);
    return success;
}

int spin_lock(spinlock_t *spinlock)
{
    if (spinlock->lock /* && held_by_this_cpu */) {
        panic_bt("spinlocks are not reentrant");
    }
    while (!spin_trylock(spinlock)) {
        asm volatile("pause");
    }
    return 1;
}

int spin_unlock(spinlock_t *spinlock)
{
    assert(spinlock->lock == 1); // spinlock wasn't taken
    atomic_store_explicit(&spinlock->lock, 0, memory_order_release);
    return 1;
}
