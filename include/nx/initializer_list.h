#pragma once

#include "nx.h"
#include <stddef.h>

#ifdef __nx_is_std
namespace std {

template <class T> class initializer_list {
    const T *m_begin;
    const T *m_end;
    const size_t m_size;

    constexpr initializer_list(const T *begin, size_t end) noexcept
        : m_begin(begin)
        , m_size(end)
    {
    }

public:
    using value_type = T;
    using reference = const T &;
    using const_reference = const T &;
    using size_type = size_t;
    using iterator = const T *;
    using const_iterator = const T *;

    constexpr initializer_list() noexcept
        : m_begin(nullptr)
        , m_size(0)
    {
    }

    constexpr iterator begin() const { return m_begin; }
    constexpr iterator end() const { return m_begin + m_size; }

    constexpr size_t size() const { return m_size; }

    constexpr const T &operator[](size_t index) const { return m_begin[index]; }
};

}
#endif // __nx_is_std

namespace NX_STD {

template <class T> constexpr const T *begin(initializer_list<T> list) noexcept
{
    return list.begin();
}

template <class T> constexpr const T *end(initializer_list<T> list) noexcept
{
    return list.end();
}

} // namespace std