
#pragma once
#ifndef NG_MUTEX_H
#define NG_MUTEX_H

#include <basic.h>
#include <nc/list.h>
#include <stdatomic.h>

struct thread;

struct kmutex {
        atomic_int mutex_value;
        struct thread *owner;
        list waitq;
};

typedef struct kmutex kmutex;
typedef struct kmutex mutex_t;

#define KMUTEX_INIT { 0, NULL, { NULL, NULL } }
#define KMUTEX_INIT_LIVE(name) do { \
        (name).mutex_value = 0; \
        (name).owner = NULL; \
        list_init(&(name).waitq); \
} while (0);


/*
 * Try to acquire the lock. If you fail, return 0 and continue, don't block.
 */
int mutex_try_lock(kmutex *lock);

/*
 * Try to acquire the lock. If you fail, block.
 */
int mutex_await(kmutex *lock);

/*
 * Release the lock -- allows one waiting thread to wake.
 */
int mutex_unlock(kmutex *lock);

/*
 * Wake `nthreads` blocking threads. Useful for condition variable modelling
 */
int mutex_signal(kmutex *lock, int nthreads);

#endif // NG_MUTEX_H

