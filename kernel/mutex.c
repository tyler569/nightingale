
#include <basic.h>
#include <stdatomic.h>
#include <stdbool.h>
#include "mutex.h"

int try_acquire_mutex(kmutex *lock) {
    atomic_bool unlocked = false;
    return atomic_compare_exchange_weak(lock, &unlocked, true);
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
    *lock = false;
    return 0;
}

