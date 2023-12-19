#pragma once

#include "thread.h"
#include <nx/atomic.h>

class mt_mutex {
public:
    mt_mutex() = default;

    mt_mutex(const mt_mutex &) = delete;
    mt_mutex &operator=(const mt_mutex &) = delete;

    mt_mutex(mt_mutex &&) = delete;
    mt_mutex &operator=(mt_mutex &&) = delete;

    void lock()
    {
        auto ticket = m_front.fetch_add(1, nx::memory_order_relaxed);
        while (m_back.load(nx::memory_order_acquire) != ticket) {
            running_thread->state = thread_state::TS_BLOCKED;
            running_thread->awaiting_newnewmutex = m_id;
            running_thread->awaiting_newnew_deli_ticket = ticket;
            thread_block();
        }
    }

    void unlock()
    {
        auto next_ticket = m_back.add_fetch(1, nx::memory_order_relaxed);
        for (auto &thread : all_threads) {
            if (thread.awaiting_newnewmutex == m_id
                && thread.awaiting_newnew_deli_ticket == next_ticket) {

                thread.state = thread_state::TS_RUNNING;
                thread.enqueue();
            }
        }
    }

private:
    static nx::atomic<int> s_next_id;

    int m_id { s_next_id++ };
    nx::atomic<unsigned> m_front { 0 };
    nx::atomic<unsigned> m_back { 0 };
};