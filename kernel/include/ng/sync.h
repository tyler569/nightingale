#pragma once

#include <list.h>
#include <ng/mutex.h>
#include <stdatomic.h>
#include <sys/cdefs.h>
#include <sys/spinlock.h>

BEGIN_DECLS

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
