
#pragma once
#ifndef _NX_TYPE_TRAITS_HH_
#define _NX_TYPE_TRAITS_HH_

#include <basic.h>

namespace nx {

template <typename T>
struct remove_extent {
    using type = T;
};

template <typename T>
struct remove_extent<T[]> {
    using type = T;
};


template <typename T>
struct remove_reference {
    using type = T;
};

template <typename T>
struct remove_reference<T&> {
    using type = T;
};

template <typename T>
struct remove_reference<T&&> {
    using type = T;
};

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

}

#endif // _NX_TYPE_TRAITS_HH_

