
#pragma once
#ifndef NG_MUTEX_H
#define NG_MUTEX_H

#include <basic.h>
#include <ng/sync.h>

typedef struct mutex kmutex;
typedef struct mutex mutex_t;

#define KMUTEX_INIT_LIVE MUTEX_CLEAR
#define KMUTEX_INIT MUTEX_INIT

#define mutex_try_lock mtx_try_lock
#define mutex_await mtx_lock
#define mutex_unlock mtx_unlock

#endif // NG_MUTEX_H

