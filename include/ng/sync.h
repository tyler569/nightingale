#pragma once
#ifndef NG_SYNC_H
#define NG_SYNC_H

#include <list.h>
#include <ng/mutex.h>
#include <stdatomic.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct spinlock {
	atomic_int lock;
};

struct ticket_spinlock {
	atomic_int front;
	atomic_int back;
};

typedef struct spinlock spinlock_t;

int spin_trylock(spinlock_t *spinlock);
int spin_lock(spinlock_t *spinlock);
int spin_unlock(spinlock_t *spinlock);

int ticket_spin_trylock(struct ticket_spinlock *spinlock);
int ticket_spin_lock(struct ticket_spinlock *spinlock);
int ticket_spin_unlock(struct ticket_spinlock *spinlock);

typedef mutex_t mutex_t;
typedef mutex_t waitqueue_t;
typedef mutex_t condvar_t;

#define wq_init mutex_init
#define wq_block_on wait_on_mutex
#define wq_notify_one wake_awaiting_thread
#define wq_notify_all wake_all_awaiting_threads

#define cv_init mutex_init
#define cv_wait wait_on_mutex_cv
#define cv_signal wake_awaiting_thread
#define cv_broadcast wake_all_awaiting_threads

#define with_lock(l) BRACKET(mutex_lock(l), mutex_unlock(l))

END_DECLS

#endif // NG_SYNC_H
