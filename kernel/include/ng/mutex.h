#pragma once

#include <stdatomic.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct mutex {
	// lock == 0, nothing reading or writing.
	// lock == -N, locked write, (N-1) waiting
	// lock == +N, locked read, (N-1) reading
	atomic_int lock;
	atomic_int ticket;
	atomic_int waiting;
	int id;
};

typedef struct mutex mutex_t;

mutex_t make_mutex();
void mutex_init(mutex_t *mutex);
bool mutex_trylock(mutex_t *mutex);
int mutex_lock(mutex_t *mutex);
int mutex_unlock(mutex_t *mutex);

void wait_on_mutex(mutex_t *mutex);
void wait_on_mutex_cv(mutex_t *condvar, mutex_t *mutex);
void wake_awaiting_thread(mutex_t *mutex);
void wake_all_awaiting_threads(mutex_t *mutex);

END_DECLS
