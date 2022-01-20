#include <basic.h>
#include <list.h>
#include <ng/sync.h>
#include <ng/thread.h>
#include <stdatomic.h>
#include <x86/interrupt.h>

// // // // //

void wq_init(struct wq *wq) {
    list_init(&wq->queue);
}

void wq_block_on(struct wq *wq) {
    block_thread(&wq->queue);
}

void wq_notify_one(struct wq *wq) {
    wake_waitq_one(&wq->queue);
}

void wq_notify_all(struct wq *wq) {
    wake_waitq_all(&wq->queue);
}


void cv_wait(struct condvar *cv, mutex_t *mtx) {
    disable_irqs();
    mtx_unlock(mtx);

    // copied-from kernel/thread.c:block_thread
    running_thread->state = TS_BLOCKED;
    list_append(&cv->wq.queue, &running_thread->wait_node);

    thread_block_irqs_disabled();
    mtx_lock(mtx);
}

void cv_signal(struct condvar *cv) {
    wq_notify_one(&cv->wq);
}

void cv_broadcast(struct condvar *cv) {
    wq_notify_all(&cv->wq);
}
