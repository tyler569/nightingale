
#include <basic.h>
#include <stdatomic.h>
#include <stdbool.h>
#include "mutex.h"
#include "kthread.h"

int try_acquire_mutex(kmutex *lock) {
    atomic_bool unlocked = false;

    if (lock->lock && current_kthread->id == lock->owner) {
        lock->refcount++;
        return true;
    }

    if (atomic_compare_exchange_weak(&lock->lock, &unlocked, true)) {
        lock->refcount++;
        lock->owner = current_kthread->id;
    }

    return true;
}

int await_mutex(kmutex *lock) {
    int t;
    while (true) {
        t = try_acquire_mutex(lock);
        if (t)
            return t;

        // asm volatile ("hlt"); // or we halt everything
    }
}

int release_mutex(kmutex *lock) {
    lock->refcount--;
    if (lock->refcount) {
        return 0;
    }
    // no remaining references in this thread
    lock->lock = false;
    return 0;
}

