#pragma once

#include <nx/atomic.h>
#include <stddef.h>

namespace nx {

class unfair_spinlock {
public:
    unfair_spinlock() = default;

    unfair_spinlock(const unfair_spinlock &) = delete;
    unfair_spinlock &operator=(const unfair_spinlock &) = delete;

    unfair_spinlock(unfair_spinlock &&) = delete;
    unfair_spinlock &operator=(unfair_spinlock &&) = delete;

    void lock()
    {
        while (m_lock.exchange(1, nx::memory_order_acquire) == 1) {
            // spin
        }
    }

    void unlock() { m_lock.store(0, nx::memory_order_release); }

private:
    nx::atomic<char> m_lock { 0 };
};

class ticket_spinlock {
public:
    ticket_spinlock() = default;

    ticket_spinlock(const ticket_spinlock &) = delete;
    ticket_spinlock &operator=(const ticket_spinlock &) = delete;

    ticket_spinlock(ticket_spinlock &&) = delete;
    ticket_spinlock &operator=(ticket_spinlock &&) = delete;

    void lock()
    {
        auto ticket = front.fetch_add(1, nx::memory_order_relaxed);
        while (back.load(nx::memory_order_acquire) != ticket) {
            // spin
        }
    }

    void unlock() { back.fetch_add(1, nx::memory_order_release); }

private:
    nx::atomic<unsigned> front { 0 };
    nx::atomic<unsigned> back { 0 };
};

using spinlock = unfair_spinlock;

}