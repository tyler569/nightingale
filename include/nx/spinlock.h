#pragma once

#include <nx/atomic.h>
#include <stddef.h>

namespace nx {

class spinlock {
public:
    spinlock() = default;
    spinlock(const spinlock &) = delete;
    spinlock(spinlock &&) = delete;
    spinlock &operator=(const spinlock &) = delete;
    spinlock &operator=(spinlock &&) = delete;

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

}