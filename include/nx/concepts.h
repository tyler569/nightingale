#pragma once
#ifndef NX_CONCEPTS_H
#define NX_CONCEPTS_H

#include "nx/utility.h"

namespace nx {

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

#endif // NX_CONCEPTS_H
