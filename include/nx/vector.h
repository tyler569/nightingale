#pragma once
#ifndef NX_VECTOR_H
#define NX_VECTOR_H

#include <nx/print.h>
#include <stddef.h>
#include <stdlib.h>

namespace nx {

template <class T> class vector {
protected:
    size_t m_len;
    size_t m_cap;
    T *m_elements;

public:
    constexpr vector()
        : m_len(0)
        , m_cap(0)
        , m_elements(nullptr)
    {
    }

    using iterator = T *;
    using const_iterator = const T *;

    iterator begin() const { return m_elements; }
    iterator end() const { return m_elements + m_len; }
    const_iterator cbegin() const { return m_elements; }
    const_iterator cend() const { return m_elements + m_len; }

    void expand(size_t new_cap)
    {
        if (new_cap < 16)
            new_cap = 16;
        if (m_cap >= new_cap)
            return;

        m_elements = (T *)realloc(m_elements, new_cap * sizeof(T *));
        m_cap = new_cap;
    }

    void push_back(const T &value)
    {
        while (m_len >= m_cap)
            expand(m_cap * 2);

        m_elements[m_len++] = value;
    }

    T &operator[](size_t index) const { return m_elements[index]; }
    [[nodiscard]] size_t size() const { return m_len; }
    [[nodiscard]] size_t capacity() const { return m_cap; }
    [[nodiscard]] T *data() { return m_elements; }
    void clear() { m_len = 0; }

    void debug_print() { print("vector: %/% d=%\n", m_len, m_cap, m_elements); }
};

}

#ifdef __nx_is_std
namespace std = nx;
#endif

#endif // NX_VECTOR_H
