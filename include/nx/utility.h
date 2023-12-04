#pragma once
#ifndef NX_UTILITY_H
#define NX_UTILITY_H

namespace nx {

template <class T> struct remove_reference {
    using type = T;
};

template <class T> struct remove_reference<T &> {
    using type = T;
};

template <class T> struct remove_reference<T &&> {
    using type = T;
};

template <class T>
using remove_reference_t = typename remove_reference<T>::type;

template <class T> constexpr remove_reference_t<T> &&move(T &&arg) noexcept
{
    return static_cast<remove_reference_t<T> &&>(arg);
}

}

#ifdef __nx_is_std
namespace std = nx;
#endif

#endif // NX_UTILITY_H
