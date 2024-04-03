#pragma once
#ifndef NX_VECTOR_H
#define NX_VECTOR_H

#include "print.h"
#include "utility.h"
#include <stddef.h>
#include <stdlib.h>

namespace nx {

template <class T> class vector {
protected:
    size_t m_size;
    size_t m_capacity;
    T *m_elements;

public:
    constexpr vector()
        : m_size(0)
        , m_capacity(0)
        , m_elements(nullptr)
    {
    }

    explicit vector(size_t len)
        : m_size(len)
        , m_capacity(len)
        , m_elements(new T[len])
    {
    }

    using iterator = T *;
    using const_iterator = const T *;

    iterator begin() const { return m_elements; }
    iterator end() const { return m_elements + m_size; }
    const_iterator cbegin() const { return m_elements; }
    const_iterator cend() const { return m_elements + m_size; }

    void expand(size_t new_cap)
    {
        if (new_cap < 16)
            new_cap = 16;
        if (m_capacity >= new_cap)
            return;

        // m_elements = (T *)realloc(m_elements, new_cap * sizeof(T *));
        T *new_elements = new T[new_cap];
        for (size_t i = 0; i < m_size; i++) {
            new_elements[i] = move(m_elements[i]);
        }
        m_elements = new_elements;
        m_capacity = new_cap;
    }

    void push_back(const T &value)
    {
        while (m_size >= m_capacity)
            expand(m_capacity * 2);

        m_elements[m_size++] = value;
    }

    void push_back(T &&value)
    {
        while (m_size >= m_capacity)
            expand(m_capacity * 2);

        m_elements[m_size++] = move(value);
    }

    template <class... Args> void emplace_back(Args &&...args)
    {
        while (m_size >= m_capacity)
            expand(m_capacity * 2);

        new (&m_elements[m_size++]) T(args...);
    }

    void pop_back()
    {
        if (m_size == 0)
            return;

        m_size--;
    }

    void pop_front()
    {
        if (m_size == 0)
            return;

        for (size_t i = 0; i < m_size - 1; i++) {
            m_elements[i] = m_elements[i + 1];
        }

        m_size--;
    }

    T &operator[](size_t index) const { return m_elements[index]; }
    [[nodiscard]] size_t size() const { return m_size; }
    [[nodiscard]] size_t capacity() const { return m_capacity; }
    [[nodiscard]] T *data() { return m_elements; }
    void clear() { m_size = 0; }
    [[nodiscard]] bool empty() const { return m_size == 0; }

    void debug_print()
    {
        print("vector: %/% d=%\n", m_size, m_capacity, m_elements);
    }
};

}

#endif // NX_VECTOR_H
