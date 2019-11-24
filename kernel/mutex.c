
#include <basic.h>
#include <ng/mutex.h>
#include <ng/panic.h>
#include <ng/print.h>
#include <ng/thread.h>
#include <stdbool.h>

int try_acquire_mutex(kmutex *lock) {
        int unlocked = 0;
        atomic_compare_exchange_weak(lock, &unlocked, running_thread->tid + 1);
        if (!unlocked) {
                // when compare exchange fails it overwrites the expected object
                // *that*'s how you know!
                return 1;
        }
        return 0;
}

int await_mutex(kmutex *lock) {
        while (true) {
                int res = try_acquire_mutex(lock);
                if (res)
                        return res;

                if (*lock == running_thread->tid + 1) {
                        printf("deadlock: waiting on mutex held by self\n");
                        assert(0);
                        // do_thread_exit?
                }

                switch_thread(SW_YIELD);
        }
}

int release_mutex(kmutex *lock) {
        *lock = 0;
        return 0;
}

