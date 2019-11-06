
#pragma once
#ifndef NG_MUTEX_H
#define NG_MUTEX_H

#include <basic.h>
#include <stdatomic.h>

typedef atomic_int kmutex;
#define KMUTEX_INIT 0

int try_acquire_mutex(kmutex *lock);
int await_mutex(kmutex *lock);
int release_mutex(kmutex *lock);

#endif // NG_MUTEX_H

