
#include <basic.h>
#include <stdatomic.h>
#include <stdbool.h>
#include "mutex.h"

static atomic_bool unlocked = false;

int try_acquire_mutex(kmutex *lock) {
    return atomic_compare_exchange_weak(lock, &unlocked, true);
}

int await_mutex(kmutex *lock) {
    int t;
    while (true) {
        t = try_acquire_mutex(lock);
        if (!t)
            return t;
    }
}

int release_mutex(kmutex *lock) {
    *lock = false;
    return 0;
}

