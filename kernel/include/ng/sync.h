#pragma once

#include <list.h>
#include <stdatomic.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct spinlock {
	atomic_int lock;
};
typedef struct spinlock spinlock_t;

struct wait_queue {
	list waiting_threads;
};
typedef struct wait_queue wq_t;

struct mutex {
	spinlock_t guard;
	bool held;

	struct wait_queue q;
};
typedef struct mutex mutex_t;

struct condvar {
	struct wait_queue q;
};
typedef struct condvar cv_t;

void spin_init(spinlock_t *spinlock);
int spin_trylock(spinlock_t *spinlock);
int spin_lock(spinlock_t *spinlock);
int spin_unlock(spinlock_t *spinlock);

void wq_init(wq_t *q);
void wq_wait(wq_t *q, spinlock_t *l);
void wq_wake_one_locked(wq_t *q);
void wq_wake_all_locked(wq_t *q);

void mutex_init(mutex_t *mutex);
void mutex_lock(mutex_t *mutex);
void mutex_unlock(mutex_t *mutex);

void cv_init(cv_t *v);
void cv_wait(cv_t *v, mutex_t *m);
void cv_wake_one_locked(cv_t *v);
void cv_wake_all_locked(cv_t *v);

END_DECLS
