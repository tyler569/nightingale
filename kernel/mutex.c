
#include <basic.h>
#include <ng/mutex.h>
#include <ng/panic.h>
#include <ng/thread.h>
#include <list.h>
#include <stdio.h>
#include <stdbool.h>

int mutex_try_lock(kmutex *lock) {
        int unlocked = 0;
        atomic_compare_exchange_strong(&lock->mutex_value, &unlocked, 1);

        if (unlocked) {
                // we lost
                return 0;
        }
        lock->owner = running_thread;
        return 1;
}

int mutex_await(kmutex *lock) {
        while (mutex_try_lock(lock) == 0) {
                block_thread(&lock->waitq);
        }
        return 1;
}

int mutex_unlock(kmutex *lock) {
        lock->mutex_value = 0;
        lock->owner = NULL;

        if (!list_empty(&lock->waitq)) {
                struct thread *blocked_head =
                        list_pop_front(struct thread, &lock->waitq, wait_node);
                wake_blocked_thread(blocked_head);
        }

        return 1;
}

int mutex_signal(kmutex *lock, int nthreads) {
        // lock->mutex_value += 1;
        return 0;
}

