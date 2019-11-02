
#pragma once
#ifndef _NX_TYPE_TRAITS_HH_
#define _NX_TYPE_TRAITS_HH_

#include <basic.h>

template <typename T>
struct remove_extent {
    using type = T;
};

template <typename T>
struct remove_extent<T[]> {
    using type = T;
};

#endif // _NX_TYPE_TRAITS_HH_

