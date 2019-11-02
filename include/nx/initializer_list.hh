
#pragma once
#ifndef _NX_INITIALIZER_LIST_HH_
#define _NX_INITIALIZER_LIST_HH_

#include <basic.h>
#include <types.hh>

namespace std {

using size_t = nx::size_t;

template <typename T>
class initializer_list {
public:
    typedef T value_type;
    typedef T const& reference;
    typedef T const& const_reference;
    typedef size_t size_type;
    typedef const T *iterator;
    typedef const T *const_iterator;

private:
    iterator array;
    size_type len;

    constexpr initializer_list(const_iterator a, size_type l)
        : array(a), len(l) {}

public:
    constexpr initializer_list() noexcept
        : array(0), len(0) {}

    constexpr size_type size() const noexcept {
        return len;
    }

    constexpr const_iterator begin() const noexcept {
        return array;
    }

    constexpr const_iterator end() const noexcept {
        return begin() + size();
    }
};

template <typename T>
constexpr const T *begin(initializer_list<T> i) noexcept;

template <typename T>
constexpr const T *end(initializer_list<T> i) noexcept;

}

#endif // _NX_INITIALIZER_LIST_HH_

