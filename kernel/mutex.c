
#include <basic.h>
#include <stdatomic.h>
#include <stdbool.h>
#include "mutex.h"


#include <stdio.h>
#include <thread.h>
#include <panic.h>

bool await_mutex(kmutex* lock) {
    while (true) {
        int unlocked = 0;
        atomic_compare_exchange_weak(lock, &unlocked, running_thread->tid + 1);
        if (!unlocked && *lock) {
            // when compare exchange fails it overwrites the expected object
            // *that*'s how you know!
            return true;
        }
        // printf("(%i:%i)", running_process->pid, running_thread->tid);
        printf("locked, I am (%i:%i), locked by: %i\n", running_process->pid, running_thread->tid, *lock-1);
        panic_bt();

        asm volatile ("pause");
    }
}

int release_mutex(kmutex* lock) {
    *lock = false;
    return 0;
}

