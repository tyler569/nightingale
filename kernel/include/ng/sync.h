#pragma once
#ifndef NG_SYNC_H
#define NG_SYNC_H

#include <basic.h>
#include <list.h>
#include <stdatomic.h>

struct wq {
        list_head queue; // struct thread.wait_node
};

#define WQ_INIT(name) { LIST_INIT(name.queue) }

// struct condvar {
//         struct wq wq;
// };

struct mutex {
        struct wq wq;
        atomic_int state;
};

#define MUTEX_INIT(name) { WQ_INIT(name.wq), 0 }
#define MUTEX_CLEAR(name) do { \
        list_init(&(name).wq); \
        name.state = 0; \
} while(0)

// struct sem {
//         struct wq wq;
//         atomic_int count;
// };

// 

// // // // //

void wq_block_on(struct wq *wq);
void wq_notify_one(struct wq *wq);
void wq_notify_all(struct wq *wq);

/*
void cv_wait(struct condvar *cv, struct mutex *mtx);
void cv_signal(struct condvar *cv);
void cv_broadcast(struct condvar *cv);
*/

int mtx_try_lock(struct mutex *mtx);
void mtx_lock(struct mutex *mtx);
void mtx_unlock(struct mutex *mtx);

#endif
