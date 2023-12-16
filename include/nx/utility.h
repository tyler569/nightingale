#pragma once
#ifndef NX_UTILITY_H
#define NX_UTILITY_H

#include <nx/type_traits.h>
#include <stddef.h>

namespace nx {

template <class T> constexpr T &&forward(remove_reference_t<T> &t) noexcept
{
    return static_cast<T &&>(t);
}

template <class T> constexpr T &&forward(remove_reference_t<T> &&t) noexcept
{
    return static_cast<T &&>(t);
}

template <class T> constexpr remove_reference_t<T> &&move(T &&arg) noexcept
{
    return static_cast<remove_reference_t<T> &&>(arg);
}

struct in_place_t {
    explicit in_place_t() = default;
};
inline constexpr in_place_t in_place {};

template <class T> struct in_place_type_t {
    explicit in_place_type_t() = default;
};
template <class T> inline constexpr in_place_type_t<T> in_place_type {};

template <size_t I> struct in_place_index_t {
    explicit in_place_index_t() = default;
};
template <size_t I> inline constexpr in_place_index_t<I> in_place_index {};

}

#endif // NX_UTILITY_H
