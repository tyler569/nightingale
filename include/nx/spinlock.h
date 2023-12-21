#pragma once

#include <ng/panic.h>
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
            // panic_bt("deadlock here");
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
        auto ticket = m_front.fetch_add(1, nx::memory_order_relaxed);
        while (m_back.load(nx::memory_order_acquire) != ticket) {
            // panic_bt("deadlock here");
            // spin
        }
    }

    void unlock() { m_back.fetch_add(1, nx::memory_order_release); }

private:
    nx::atomic<unsigned> m_front { 0 };
    nx::atomic<unsigned> m_back { 0 };
};

using spinlock = ticket_spinlock;

}