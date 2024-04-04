#pragma once

#include <nx/type_traits.h>

namespace nx {

template <class T> class reverse_iterator {
    T m_it;

public:
    using result_type = nx::remove_reference_t<decltype(*m_it)>;

    explicit constexpr reverse_iterator(T it)
        : m_it(it)
    {
    }

    result_type &operator*() { return *(m_it - 1); }
    result_type *operator->() { return m_it - 1; }

    reverse_iterator &operator++()
    {
        m_it--;
        return *this;
    }

    reverse_iterator &operator--()
    {
        m_it++;
        return *this;
    }

    reverse_iterator operator++(int)
    {
        reverse_iterator tmp = *this;
        m_it--;
        return tmp;
    }

    reverse_iterator operator--(int)
    {
        reverse_iterator tmp = *this;
        m_it++;
        return tmp;
    }

    reverse_iterator operator+(int n)
    {
        reverse_iterator tmp = *this;
        tmp.m_it -= n;
        return tmp;
    }

    reverse_iterator operator-(int n)
    {
        reverse_iterator tmp = *this;
        tmp.m_it += n;
        return tmp;
    }

    reverse_iterator &operator+=(int n)
    {
        m_it -= n;
        return *this;
    }

    reverse_iterator &operator-=(int n)
    {
        m_it += n;
        return *this;
    }

    bool operator==(const reverse_iterator &other) const
    {
        return m_it == other.m_it;
    }

    int operator<=>(const reverse_iterator &other) const
    {
        return m_it <=> other.m_it;
    }
};

}

