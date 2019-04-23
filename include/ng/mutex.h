
#ifndef NIGHTINGALE_MUTEX_H
#define NIGHTINGALE_MUTEX_H

#include <ng/basic.h>
#include <stdatomic.h>

typedef atomic_int kmutex;
#define KMUTEX_INIT 0

int try_acquire_mutex(kmutex *lock);
int await_mutex(kmutex *lock);
int release_mutex(kmutex *lock);

#endif
