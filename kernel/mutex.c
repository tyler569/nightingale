
#include <basic.h>
#include <stdatomic.h>
#include <stdbool.h>
#include "mutex.h"

#include "print.h" // tmptmptmpt


bool await_mutex(kmutex* lock) {
    // printf("Trying to take lock %#lx\n", lock);
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
    // printf("Releasing lock %#lx\n", lock);
    *lock = false;
    return 0;
}

