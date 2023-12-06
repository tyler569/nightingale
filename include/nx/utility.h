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

template <class T> struct is_lvalue_reference {
    static constexpr bool value = false;
};

template <class T> struct is_lvalue_reference<T &> {
    static constexpr bool value = true;
};

template <class T>
constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

template <class T> struct is_rvalue_reference {
    static constexpr bool value = false;
};

template <class T> struct is_rvalue_reference<T &&> {
    static constexpr bool value = true;
};

template <class T>
constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

template <class T> struct is_integral {
    static constexpr bool value = false;
};

template <> struct is_integral<bool> {
    static constexpr bool value = true;
};

template <> struct is_integral<char> {
    static constexpr bool value = true;
};

template <> struct is_integral<signed char> {
    static constexpr bool value = true;
};

template <> struct is_integral<unsigned char> {
    static constexpr bool value = true;
};

template <> struct is_integral<short> {
    static constexpr bool value = true;
};

template <> struct is_integral<unsigned short> {
    static constexpr bool value = true;
};

template <> struct is_integral<int> {
    static constexpr bool value = true;
};

template <> struct is_integral<unsigned int> {
    static constexpr bool value = true;
};

template <> struct is_integral<long> {
    static constexpr bool value = true;
};

template <> struct is_integral<unsigned long> {
    static constexpr bool value = true;
};

template <> struct is_integral<long long> {
    static constexpr bool value = true;
};

template <> struct is_integral<unsigned long long> {
    static constexpr bool value = true;
};

template <class T> constexpr bool is_integral_v = is_integral<T>::value;

template <class T>
concept is_integral_c = is_integral_v<T>;

template <class T> struct is_pointer {
    static constexpr bool value = false;
};

template <class T> struct is_pointer<T *> {
    static constexpr bool value = true;
};

template <class T> constexpr bool is_pointer_v = is_pointer<T>::value;

template <class T, class U> struct is_same {
    static constexpr bool value = false;
};

template <class T> struct is_same<T, T> {
    static constexpr bool value = true;
};

template <class T, class U> constexpr bool is_same_v = is_same<T, U>::value;

template <class T, class U> struct is_base_of {
    static constexpr bool value = __is_base_of(T, U);
};

template <class T, class U>
constexpr bool is_base_of_v = is_base_of<T, U>::value;

template <class T> struct is_class {
    static constexpr bool value = __is_class(T);
};

template <class T> constexpr bool is_class_v = is_class<T>::value;

template <class T> struct is_function {
    static constexpr bool value = __is_function(T);
};

template <class T> constexpr bool is_function_v = is_function<T>::value;

}

#endif // NX_UTILITY_H
