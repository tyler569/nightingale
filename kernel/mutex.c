
#include <basic.h>
#include <ng/mutex.h>
#include <ng/panic.h>
#include <ng/thread.h>
#include <list.h>
#include <stdio.h>
#include <stdbool.h>

int mutex_try_lock(kmutex *lock) {
        // printf("mutex: thread %i is trying to take lock %p\n", running_thread->tid, lock);
        int unlocked = 0;
        atomic_compare_exchange_strong(&lock->mutex_value, &unlocked, 1);

        if (unlocked) {
                // we lost
                return 0;
        }
        // TODO: recurseiveness?
        lock->owner = running_thread;
        return 1;
}

int mutex_await(kmutex *lock) {
        while (mutex_try_lock(lock) == 0) {
                // printf("mutex %p: thread %i lost - blocking\n", lock, running_thread->tid);
                block_thread(&lock->waitq);
        }
        return 1;
}

int mutex_unlock(kmutex *lock) {
        // printf("mutex: thread %i is unlocking lock %p\n", running_thread->tid, lock);
        lock->mutex_value = 0;
        lock->owner = NULL;

        wake_waitq_one(&lock->waitq);

        return 1;
}

int mutex_signal(kmutex *lock, int nthreads) {
        printf("mutex: signal is TODO\n");
        return 0;
}

