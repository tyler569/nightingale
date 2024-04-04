#pragma once

#include "assert.h"
#include "nx/print.h"
#include "vmm.h"
#include <nx/algorithm.h>
#include <nx/optional.h>
#include <nx/spinlock.h>
#include <sys/cdefs.h>
#include <sys/types.h>

template <class T> class spalloc {
public:
    constexpr spalloc() = default;
    explicit spalloc(size_t capacity);

    spalloc(const spalloc &) = delete;
    spalloc &operator=(const spalloc &) = delete;

    // todo: this is moveable
    spalloc(spalloc &&) = delete;
    spalloc &operator=(spalloc &&) = delete;

    void init(size_t capacity = 0x10000);

    T *alloc_uninit();
    T *move_store(T &&object);
    template <class... Args> T *emplace(Args &&...args);
    void free(T *);

private:
    T *region;
    nx::optional<size_t> free_list_head {};
    size_t bump_free { 0 };
    size_t count { 0 };
    size_t capacity { 0 };
    nx::spinlock lock {};

    static size_t offset()
    {
        return (sizeof(T) < sizeof(T *) ? sizeof(T *) : sizeof(T));
    }

    T *get(size_t n)
    {
        return reinterpret_cast<T *>((char *)region + offset() * n);
    }

    T *free_list_pop();
    void free_list_push(T *item);
    T *bump_pop();
};

template <class T>
spalloc<T>::spalloc(size_t capacity)
    : region((T *)vmm_reserve(capacity * nx::max(sizeof(T), sizeof(T *))))
    , capacity(capacity)
{
}

template <class T> void spalloc<T>::init(size_t cap)
{
    capacity = cap;
    region = (T *)vmm_reserve(capacity * nx::max(sizeof(T), sizeof(T *)));
}

template <class T> T *spalloc<T>::free_list_pop()
{
    if (free_list_head) {
        auto *result = get(free_list_head.value());
        auto free_list_next = *(size_t *)result;
        if (free_list_next == SIZE_MAX) {
            free_list_head = nx::nullopt;
        } else {
            free_list_head = free_list_next;
        }
        return result;
    }
    return nullptr;
}

template <class T> void spalloc<T>::free_list_push(T *item)
{
    auto index = ((char *)item - (char *)region) / offset();
    *(size_t *)item = free_list_head.value_or(SIZE_MAX);
    free_list_head = index;
}

template <class T> T *spalloc<T>::bump_pop()
{
    assert(bump_free < capacity);
    return get(bump_free++);
}

template <class T> T *spalloc<T>::alloc_uninit()
{
    lock.lock();
    count += 1;
    if (count == capacity) {
        panic_bt("sp_alloc exhausted!\n");
    }

    auto *free_list_result = free_list_pop();
    if (free_list_result) {
        lock.unlock();
        return free_list_result;
    }
    auto *bump_result = bump_pop();
    lock.unlock();
    return bump_result;
}

template <class T> T *spalloc<T>::move_store(T &&object)
{
    auto allocation = alloc_uninit();
    *allocation = move(object);
};

template <class T>
template <class... Args>
T *spalloc<T>::emplace(Args &&...args)
{
    auto *allocation = alloc_uninit();
    return new (allocation) T(args...);
}

template <class T> void spalloc<T>::free(T *item)
{
    lock.lock();
    memset(item, 'G', offset());
    free_list_push(item);
    lock.unlock();
}

