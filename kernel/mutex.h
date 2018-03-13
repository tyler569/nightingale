
#ifndef NIGHTINGALE_MUTEX_H
#define NIGHTINGALE_MUTEX_H

#include <basic.h>
#include <stdatomic.h>

typedef atomic_bool kmutex;

#define KMUTEX_INIT false;

int try_acquire_mutex(kmutex *lock);
int await_mutex(kmutex *lock);
int release_mutex(kmutex *lock);

#endif

