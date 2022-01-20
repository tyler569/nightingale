#pragma once
#ifndef NG_SYNC_H
#define NG_SYNC_H

#include <basic.h>
#include <list.h>
#include <stdatomic.h>
#include <ng/newmutex.h>

struct wq {
    list_head queue; // struct thread.wait_node
};

#define WQ_INIT(name)                                                          \
    { .queue = LIST_INIT(name.queue) }

struct condvar {
    struct wq wq;
};

#define CV_INIT(name)                                                          \
    { .wq = WQ_INIT(name.wq) }

// typedef struct mutex mutex_t;
typedef newmutex_t mutex_t;

struct sem {
    struct wq wq;
    atomic_int count;
};

#define SEM_INIT(name)                                                         \
    { .wq = WQ_INIT, .count = 0 }

//

// // // // //

void wq_init(struct wq *wq);
void wq_block_on(struct wq *wq);
void wq_notify_one(struct wq *wq);
void wq_notify_all(struct wq *wq);
void cv_wait(struct condvar *cv, mutex_t *mtx);
void cv_signal(struct condvar *cv);
void cv_broadcast(struct condvar *cv);

#define mutex_try_lock newmutex_trylock
#define mutex_lock newmutex_lock
#define mutex_unlock newmutex_unlock
#define mutex_init newmutex_init
#define make_mutex make_newmutex

#define mutex_await mutex_lock

#define with_lock(l) BRACKET(mutex_lock(l), mutex_unlock(l))

#endif // NG_SYNC_H
