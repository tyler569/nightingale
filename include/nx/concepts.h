#pragma once

#include "nx.h"
#include "utility.h"

namespace NX_STD {

template <class T>
concept integral = is_integral_v<T>;

template <class T>
concept pointer = is_pointer_v<T>;

template <class T, class U>
concept same_as = is_same_v<T, U> && is_same_v<U, T>;

template <class T, class U>
concept derived_from = is_base_of_v<U, T>;

template <class T>
concept class_type = is_class_v<T>;

}
