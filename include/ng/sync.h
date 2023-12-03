#pragma once
#ifndef NG_SYNC_H
#define NG_SYNC_H

#include <list.h>
#include <ng/newmutex.h>
#include <stdatomic.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct spinlock {
    atomic_int lock;
    atomic_int held_by_cpu;
};

typedef struct spinlock spinlock_t;

int spin_trylock(spinlock_t *spinlock);
int spin_lock(spinlock_t *spinlock);
int spin_unlock(spinlock_t *spinlock);

typedef newmutex_t mutex_t;
typedef newmutex_t waitqueue_t;
typedef newmutex_t condvar_t;

#define mutex_trylock newmutex_trylock
#define mutex_lock newmutex_lock
#define mutex_unlock newmutex_unlock
#define mutex_init newmutex_init
#define make_mutex make_newmutex
#define mutex_await mutex_lock

#define wq_init newmutex_init
#define wq_block_on wait_on_newmutex
#define wq_notify_one wake_awaiting_thread
#define wq_notify_all wake_all_awaiting_threads

#define cv_init newmutex_init
#define cv_wait wait_on_newmutex_cv
#define cv_signal wake_awaiting_thread
#define cv_broadcast wake_all_awaiting_threads

#define with_lock(l) BRACKET(mutex_lock(l), mutex_unlock(l))

END_DECLS

#endif // NG_SYNC_H
