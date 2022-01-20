#include <basic.h>
#include <stdatomic.h>
#include <ng/thread.h>
#include <ng/newmutex.h>

atomic_int next_mutex_id = 1;

void newmutex_init(newmutex_t *newmutex) {
    int id = atomic_fetch_add_explicit(&next_mutex_id, 1, memory_order_relaxed);
    newmutex->lock = 0;
    newmutex->ticket = 0;
    newmutex->id = id;
}

bool newmutex_trylock(newmutex_t *newmutex) {
    int expected = 0;
    int desired = -1;
    // fast path
    if (newmutex->lock != expected)  return false;
    return atomic_compare_exchange_weak_explicit(
            &newmutex->lock, &expected, desired,
            memory_order_acquire,
            memory_order_relaxed);
}

static void wait_on_newmutex(newmutex_t *newmutex) {
    disable_irqs();
    running_thread->state = TS_BLOCKED;
    running_thread->awaiting_newmutex = newmutex->id;
    int ticket =
        atomic_fetch_add_explicit(&newmutex->ticket, 1, memory_order_relaxed);
    running_thread->awaiting_deli_ticket = ticket;
    atomic_fetch_add_explicit(&newmutex->waiting, 1, memory_order_relaxed);
    thread_block_irqs_disabled();
}

int newmutex_lock(newmutex_t *newmutex) {
    while (!newmutex_trylock(newmutex)) {
        wait_on_newmutex(newmutex);
    }
    return true;
}

static void wake_awaiting_thread(newmutex_t *newmutex) {
    // fast path
    if (!newmutex->waiting)  return;
    int n_awaiting = 0;
    int best_deli_ticket = INT_MAX;
    struct thread *winning_thread = NULL;
    list_for_each(struct thread, th, &all_threads, all_threads) {
        if (th->state != TS_BLOCKED)  continue;
        if (th->awaiting_newmutex != newmutex->id)  continue;

        n_awaiting += 1;
        if (best_deli_ticket > th->awaiting_deli_ticket) {
            best_deli_ticket = th->awaiting_deli_ticket;
            winning_thread = th;
        }
    }
    newmutex->waiting = n_awaiting; // racey
    if (!winning_thread)  return;
    winning_thread->state = TS_RUNNING;
    winning_thread->awaiting_newmutex = 0;
    thread_enqueue(winning_thread);
}

int newmutex_unlock(newmutex_t *newmutex) {
    atomic_store_explicit(&newmutex->lock, 0, memory_order_release);
    wake_awaiting_thread(newmutex);
    return true;
}
