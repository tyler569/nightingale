#pragma once

#include "utility.h"
#include <stddef.h>

namespace nx {

template <class T, class U> class pair {
public:
    T first;
    U second;

    pair() = default;

    pair(const T &first, const U &second)
        : first(first)
        , second(second)
    {
    }

    pair(T &&first, U &&second)
        : first(first)
        , second(second)
    {
    }

    pair(const pair &other)
        : first(other.first)
        , second(other.second)
    {
    }

    pair(pair &&other)
        : first(move(other.first))
        , second(move(other.second))
    {
    }

    pair &operator=(const pair &other)
    {
        first = other.first;
        second = other.second;
        return *this;
    }

    pair &operator=(pair &&other)
    {
        first = move(other.first);
        second = move(other.second);
        return *this;
    }

    ~pair() = default;
};

}
