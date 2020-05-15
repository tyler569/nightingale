
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

        // very debug only
        // assert(list_empty(&lock->waitq));

        if (!list_empty(&lock->waitq)) {
                struct thread *blocked_head =
                        list_pop_front(struct thread, &lock->waitq, wait_node);
                wake_blocked_thread(blocked_head);
        }

        return 1;
}

int mutex_signal(kmutex *lock, int nthreads) {
        printf("mutex: signal is TODO\n");
        return 0;
}

