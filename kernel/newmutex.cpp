#include <assert.h>
#include <ng/common.h>
#include <ng/mt/thread.h>
#include <ng/newmutex.h>
#include <ng/thread.h>
#include <stdatomic.h>

atomic_int next_mutex_id = 1;

void newmutex_init(newmutex_t *newmutex)
{
    int id = atomic_fetch_add_explicit(&next_mutex_id, 1, memory_order_relaxed);
    newmutex->lock = 0;
    newmutex->ticket = 0;
    newmutex->id = id;
}

mutex_t make_newmutex()
{
    mutex_t tmp;
    mutex_init(&tmp);
    return tmp;
}

bool newmutex_trylock(newmutex_t *newmutex)
{
    assert(newmutex->id != 0);

    int expected = 0;
    int desired = -1;
    // fast path
    if (newmutex->lock != expected)
        return false;
    return atomic_compare_exchange_weak_explicit(&newmutex->lock, &expected,
        desired, memory_order_acquire, memory_order_relaxed);
}

// Alternate form to operate as a condition variable, seeing if we can use
// the same guts for both.
void wait_on_newmutex_cv(newmutex_t *condvar, newmutex_t *mutex)
{
    assert(condvar->id != 0);

    if (mutex)
        mutex_unlock(mutex);

    running_thread->state = TS_BLOCKED;
    running_thread->awaiting_newmutex = condvar->id;
    int ticket
        = atomic_fetch_add_explicit(&condvar->ticket, 1, memory_order_relaxed);
    running_thread->awaiting_deli_ticket = ticket;
    atomic_fetch_add_explicit(&condvar->waiting, 1, memory_order_relaxed);
    thread_block();

    if (mutex)
        mutex_lock(mutex);
}

void wait_on_newmutex(newmutex_t *newmutex)
{
    wait_on_newmutex_cv(newmutex, nullptr);
}

int newmutex_lock(newmutex_t *newmutex)
{
    while (!newmutex_trylock(newmutex)) {
        wait_on_newmutex(newmutex);
    }
    return true;
}

void wake_awaiting_thread(newmutex_t *newmutex)
{
    assert(newmutex->id != 0);

    // fast path
    if (!newmutex->waiting)
        return;
    int n_awaiting = 0;
    int best_deli_ticket = INT_MAX;
    struct thread *winning_thread = nullptr;
    for (auto &thread : all_threads) {
        if (thread.state != TS_BLOCKED)
            continue;
        if (thread.awaiting_newmutex != newmutex->id)
            continue;

        n_awaiting += 1;
        if (best_deli_ticket > thread.awaiting_deli_ticket) {
            best_deli_ticket = thread.awaiting_deli_ticket;
            winning_thread = &thread;
        }
    }
    newmutex->waiting = n_awaiting; // racey
    if (!winning_thread)
        return;
    winning_thread->state = TS_RUNNING;
    winning_thread->awaiting_newmutex = 0;
    thread_enqueue(winning_thread);
}

// This doesn't make a lot of sense for a mutex, but I'm trying to see if I
// can use the same guts for a wait queue, and this does make a lot of sense
// there.
void wake_all_awaiting_threads(newmutex_t *newmutex)
{
    // fast path
    if (!newmutex->waiting)
        return;
    for (auto &thread : all_threads) {
        if (thread.state != TS_BLOCKED)
            continue;
        if (thread.awaiting_newmutex != newmutex->id)
            continue;

        thread.state = TS_RUNNING;
        thread.awaiting_newmutex = 0;
        thread_enqueue(&thread);
    }
    newmutex->waiting = 0;
}

int newmutex_unlock(newmutex_t *newmutex)
{
    atomic_store_explicit(&newmutex->lock, 0, memory_order_release);
    wake_awaiting_thread(newmutex);
    return true;
}
