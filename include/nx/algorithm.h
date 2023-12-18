#pragma once

namespace nx {

template <class T> const T &max(const T &a, const T &b)
{
    return (a < b) ? b : a;
}

template <class T> const T &min(const T &a, const T &b)
{
    return (a < b) ? a : b;
}

}