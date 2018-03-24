
#ifndef NIGHTINGALE_MUTEX_H
#define NIGHTINGALE_MUTEX_H

#include <basic.h>
#include <stdatomic.h>
#include <kthread.h>

/*struct kmutex {
    atomic_bool lock;
    pid_t owner;
    int refcount;
};

typedef volatile struct kmutex kmutex;

#define KMUTEX_INIT { false, -1, 0 };
*/

typedef volatile atomic_bool kmutex;
#define KMUTEX_INIT false;

int try_acquire_mutex(kmutex *lock);
int await_mutex(kmutex *lock);
int release_mutex(kmutex *lock);

#endif

