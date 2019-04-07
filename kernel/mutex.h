
#ifndef NIGHTINGALE_MUTEX_H
#define NIGHTINGALE_MUTEX_H

#include <basic.h>
#include <stdatomic.h>

typedef atomic_int kmutex;
#define KMUTEX_INIT 0

bool await_mutex(kmutex* lock);
int release_mutex(kmutex* lock);

#endif

