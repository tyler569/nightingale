#pragma once
#ifndef NX_UTILITY_H
#define NX_UTILITY_H

#include "nx.h"
#include "type_traits.h"
#include <stddef.h>

namespace NX_STD {

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

}

#endif // NX_UTILITY_H
