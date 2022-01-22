#pragma once
#ifndef NG_SYNC_H
#define NG_SYNC_H

#include <basic.h>
#include <list.h>
#include <stdatomic.h>
#include <ng/newmutex.h>

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

#endif // NG_SYNC_H
