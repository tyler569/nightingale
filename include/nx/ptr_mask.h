#pragma once

#include "type_traits.h"

namespace nx {

template <class... Args> constexpr unsigned ptr_mask = 0;

template <class Arg, class... Args>
constexpr unsigned ptr_mask<Arg, Args...>
    = ptr_mask<Arg> | (ptr_mask<Args...> << 1);

template <class T>
    requires nx::is_pointer_v<T>
constexpr unsigned ptr_mask<T> = 1;

template <class T> constexpr unsigned ptr_mask<T> = 0;

template <class F> struct function_ptr_mask;

template <class R, class... Args> struct function_ptr_mask<R(Args...)> {
    static constexpr unsigned value = ptr_mask<Args...>;
};

template <class R, class... Args> struct function_ptr_mask<R (*)(Args...)> {
    static constexpr unsigned value = ptr_mask<Args...>;
};

template <class C, class R, class... Args>
struct function_ptr_mask<R (C::*)(Args...)> {
    static constexpr unsigned value = ptr_mask<Args...>;
};

template <class C, class R, class... Args>
struct function_ptr_mask<R (C::*)(Args...) const> {
    static constexpr unsigned value = ptr_mask<Args...>;
};

template <class R, class... Args> struct function_ptr_mask<R(Args..., ...)> {
    static constexpr unsigned value = ptr_mask<Args...>;
};

template <class R, class... Args>
struct function_ptr_mask<R (*)(Args..., ...)> {
    static constexpr unsigned value = ptr_mask<Args...>;
};

template <class C, class R, class... Args>
struct function_ptr_mask<R (C::*)(Args..., ...)> {
    static constexpr unsigned value = ptr_mask<Args...>;
};

template <class C, class R, class... Args>
struct function_ptr_mask<R (C::*)(Args..., ...) const> {
    static constexpr unsigned value = ptr_mask<Args...>;
};

template <class L>
    requires requires { &L::operator(); }
struct function_ptr_mask<L> {
    static constexpr unsigned value
        = function_ptr_mask<decltype(&L::operator())>::value;
};

template <auto F>
constexpr unsigned function_ptr_mask_v = function_ptr_mask<decltype(F)>::value;

}