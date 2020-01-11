
#pragma once
#ifndef _NX_UTILITY_HH_
#define _NX_UTILITY_HH_

#include <type_traits.hh>

namespace nx {

template <typename T>
remove_reference_t<T>&& move(T&& v) {
    return v;
}

}

#endif // _NX_UTILITY_HH_

