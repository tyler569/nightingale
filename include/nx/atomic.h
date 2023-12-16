#pragma once
#ifndef NX_ATOMIC_H
#define NX_ATOMIC_H

#include <nx/concepts.h>

namespace nx {

enum memory_order {
    memory_order_relaxed = __ATOMIC_RELAXED,
    memory_order_consume = __ATOMIC_CONSUME,
    memory_order_acquire = __ATOMIC_ACQUIRE,
    memory_order_release = __ATOMIC_RELEASE,
    memory_order_acq_rel = __ATOMIC_ACQ_REL,
    memory_order_seq_cst = __ATOMIC_SEQ_CST,
};

template <class T>
    requires integral<T> || pointer<T>
class atomic {
public:
    constexpr atomic() noexcept
        : value(0)
    {
    }

    constexpr atomic(T desired) noexcept
        : value(desired)
    {
    }

    T load(memory_order ordering = memory_order_seq_cst) const noexcept
    {
        return __atomic_load_n(&value, ordering);
    }

    void store(T desired, memory_order ordering = memory_order_seq_cst) noexcept
    {
        __atomic_store_n(&value, desired, ordering);
    }

    T exchange(T desired, memory_order ordering = memory_order_seq_cst) noexcept
    {
        return __atomic_exchange_n(&value, desired, ordering);
    }

    bool compare_exchange_weak(T &expected, T desired, memory_order success,
        memory_order failure) noexcept
    {
        return __atomic_compare_exchange_n(
            &value, &expected, desired, true, success, failure);
    }

    bool compare_exchange_weak(T &expected, T desired,
        memory_order ordering = memory_order_seq_cst) noexcept
    {
        return compare_exchange_weak(expected, desired, ordering, ordering);
    }

    bool compare_exchange_strong(T &expected, T desired, memory_order success,
        memory_order failure) noexcept
    {
        return __atomic_compare_exchange_n(
            &value, &expected, desired, false, success, failure);
    }

    bool compare_exchange_strong(T &expected, T desired,
        memory_order ordering = memory_order_seq_cst) noexcept
    {
        return compare_exchange_strong(expected, desired, ordering, ordering);
    }

    T fetch_add(T arg, memory_order ordering = memory_order_seq_cst) noexcept
    {
        return __atomic_fetch_add(&value, arg, ordering);
    }

    T add_fetch(T arg, memory_order ordering = memory_order_seq_cst) noexcept
    {
        return __atomic_add_fetch(&value, arg, ordering);
    }

    T fetch_sub(T arg, memory_order ordering = memory_order_seq_cst) noexcept
    {
        return __atomic_fetch_sub(&value, arg, ordering);
    }

    T sub_fetch(T arg, memory_order ordering = memory_order_seq_cst) noexcept
    {
        return __atomic_sub_fetch(&value, arg, ordering);
    }

    T fetch_and(T arg, memory_order ordering = memory_order_seq_cst) noexcept
    {
        return __atomic_fetch_and(&value, arg, ordering);
    }

    T and_fetch(T arg, memory_order ordering = memory_order_seq_cst) noexcept
    {
        return __atomic_and_fetch(&value, arg, ordering);
    }

    T fetch_or(T arg, memory_order ordering = memory_order_seq_cst) noexcept
    {
        return __atomic_fetch_or(&value, arg, ordering);
    }

    T or_fetch(T arg, memory_order ordering = memory_order_seq_cst) noexcept
    {
        return __atomic_or_fetch(&value, arg, ordering);
    }

    T fetch_xor(T arg, memory_order ordering = memory_order_seq_cst) noexcept
    {
        return __atomic_fetch_xor(&value, arg, ordering);
    }

    T xor_fetch(T arg, memory_order ordering = memory_order_seq_cst) noexcept
    {
        return __atomic_xor_fetch(&value, arg, ordering);
    }

    T fetch_nand(T arg, memory_order ordering = memory_order_seq_cst) noexcept
    {
        return __atomic_fetch_nand(&value, arg, ordering);
    }

    T nand_fetch(T arg, memory_order ordering = memory_order_seq_cst) noexcept
    {
        return __atomic_nand_fetch(&value, arg, ordering);
    }

    T operator++(int) noexcept { return fetch_add(1); }
    T operator--(int) noexcept { return fetch_sub(1); }
    T operator++() noexcept { return add_fetch(1); }
    T operator--() noexcept { return sub_fetch(1); }
    T operator+=(T arg) noexcept { return add_fetch(arg); }
    T operator-=(T arg) noexcept { return sub_fetch(arg); }
    T operator&=(T arg) noexcept { return and_fetch(arg); }
    T operator|=(T arg) noexcept { return or_fetch(arg); }
    T operator^=(T arg) noexcept { return xor_fetch(arg); }

    operator T() const noexcept { return load(); } // NOLINT

    atomic &operator=(T desired) noexcept
    {
        store(desired);
        return *this;
    }

private:
    T value;
};

}

#endif // NX_ATOMIC_H
