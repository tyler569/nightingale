
#include <basic.h>
#include <stdatomic.h>
#include <stdbool.h>
#include "mutex.h"

bool await_mutex(kmutex* lock) {
    while (true) {
        atomic_bool unlocked = false;
        atomic_compare_exchange_weak(lock, &unlocked, true);
        if (!unlocked && *lock) {
            // when compare exchange fails it overwrites the expected object
            // *that*'s how you know!
            return true;
        }
    }
}

int release_mutex(kmutex* lock) {
    *lock = false;
    return 0;
}

