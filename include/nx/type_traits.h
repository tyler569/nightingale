#pragma once

#include <stddef.h>

namespace nx {

// clang-format off

//
// IS_XXX
//

template <class T> static constexpr bool is_void_v = false;
template <> static constexpr bool is_void_v<void> = true;
template <class T> static constexpr bool is_null_pointer_v = false;
template <> static constexpr bool is_null_pointer_v<decltype(nullptr)> = true;
template <class T> static constexpr bool is_integral_v = false;
template <> static constexpr bool is_integral_v<bool> = true;
template <> static constexpr bool is_integral_v<char> = true;
template <> static constexpr bool is_integral_v<signed char> = true;
template <> static constexpr bool is_integral_v<unsigned char> = true;
template <> static constexpr bool is_integral_v<short> = true;
template <> static constexpr bool is_integral_v<unsigned short> = true;
template <> static constexpr bool is_integral_v<int> = true;
template <> static constexpr bool is_integral_v<unsigned int> = true;
template <> static constexpr bool is_integral_v<long> = true;
template <> static constexpr bool is_integral_v<unsigned long> = true;
template <> static constexpr bool is_integral_v<long long> = true;
template <> static constexpr bool is_integral_v<unsigned long long> = true;
template <class T> static constexpr bool is_floating_point_v = false;
template <> static constexpr bool is_floating_point_v<float> = true;
template <> static constexpr bool is_floating_point_v<double> = true;
template <> static constexpr bool is_floating_point_v<long double> = true;
template <class T> static constexpr bool is_array_v = false;
template <class T> static constexpr bool is_array_v<T[]> = true;
template <class T, size_t N> static constexpr bool is_array_v<T[N]> = true;
template <class T> static constexpr bool is_enum_v = __is_enum(T);
template <class T> static constexpr bool is_union_v = __is_union(T);
template <class T> static constexpr bool is_class_v = __is_class(T);
template <class T> static constexpr bool is_function_v = __is_function(T);
template <class T> static constexpr bool is_pointer_v = false;
template <class T> static constexpr bool is_pointer_v<T *> = true;
template <class T> static constexpr bool is_lvalue_reference_v = false;
template <class T> static constexpr bool is_lvalue_reference_v<T &> = true;
template <class T> static constexpr bool is_rvalue_reference_v = false;
template <class T> static constexpr bool is_rvalue_reference_v<T &&> = true;
template <class T> static constexpr bool is_member_object_pointer_v = false;
template <class T, class U> static constexpr bool is_member_object_pointer_v<T U::*> = true;
template <class T> static constexpr bool is_member_function_pointer_v = false;
template <class T, class U> static constexpr bool is_member_function_pointer_v<T U::*> = true;
template <class T> static constexpr bool is_member_pointer_v = is_member_object_pointer_v<T> || is_member_function_pointer_v<T>;
template <class T> static constexpr bool is_fundamental_v = is_integral_v<T> || is_floating_point_v<T> || is_void_v<T>;
template <class T> static constexpr bool is_arithmetic_v = is_integral_v<T> || is_floating_point_v<T>;
template <class T> static constexpr bool is_scalar_v = is_arithmetic_v<T> || is_enum_v<T> || is_pointer_v<T> || is_member_pointer_v<T>;
template <class T> static constexpr bool is_object_v = is_scalar_v<T> || is_array_v<T> || is_union_v<T> || is_class_v<T>;
template <class T> static constexpr bool is_reference_v = is_lvalue_reference_v<T> || is_rvalue_reference_v<T>;
template <class T> static constexpr bool is_compound_v = is_array_v<T> || is_pointer_v<T> || is_reference_v<T> || is_class_v<T> || is_union_v<T> || is_enum_v<T>;
template <class T> static constexpr bool is_const_v = false;
template <class T> static constexpr bool is_const_v<const T> = true;
template <class T> static constexpr bool is_volatile_v = false;
template <class T> static constexpr bool is_volatile_v<volatile T> = true;
template <class T> static constexpr bool is_trivial_v = __is_trivial(T);
template <class T> static constexpr bool is_trivially_copyable_v = __is_trivially_copyable(T);
template <class T> static constexpr bool is_standard_layout_v = __is_standard_layout(T);
template <class T> static constexpr bool is_pod_v = is_trivial_v<T> && is_standard_layout_v<T>;
template <class T> static constexpr bool is_empty_v = __is_empty(T);
template <class T> static constexpr bool is_polymorphic_v = __is_polymorphic(T);
template <class T> static constexpr bool is_abstract_v = __is_abstract(T);
template <class T> static constexpr bool is_final_v = __is_final(T);
template <class T> static constexpr bool is_signed_v = __is_signed(T);
template <class T> static constexpr bool is_unsigned_v = __is_unsigned(T);
template <class T, class... Args> static constexpr bool is_constructible_v = __is_constructible(T, Args...);
template <class T> static constexpr bool is_default_constructible_v = __is_constructible(T);
template <class T> static constexpr bool is_copy_constructible_v = __is_constructible(T, const T &);
template <class T> static constexpr bool is_move_constructible_v = __is_constructible(T, T &&);
template <class T, class U> static constexpr bool is_assignable_v = __is_assignable(T, U);
template <class T> static constexpr bool is_copy_assignable_v = __is_assignable(T &, const T &);
template <class T> static constexpr bool is_move_assignable_v = __is_assignable(T &, T &&);
template <class T> static constexpr bool is_destructible_v = __is_destructible(T);
template <class T, class... Args> static constexpr bool is_trivially_constructible_v = __is_trivially_constructible(T, Args...);
template <class T> static constexpr bool is_trivially_default_constructible_v = __is_trivially_constructible(T);
template <class T> static constexpr bool is_trivially_copy_constructible_v = __is_trivially_constructible(T, const T &);
template <class T> static constexpr bool is_trivially_move_constructible_v = __is_trivially_constructible(T, T &&);
template <class T, class U> static constexpr bool is_trivially_assignable_v = __is_trivially_assignable(T, U);
template <class T> static constexpr bool is_trivially_copy_assignable_v = __is_trivially_assignable(T &, const T &);
template <class T> static constexpr bool is_trivially_move_assignable_v = __is_trivially_assignable(T &, T &&);
template <class T> static constexpr bool is_trivially_destructible_v = __is_trivially_destructible(T);
template <class T, class... Args> static constexpr bool is_nothrow_constructible_v = __is_nothrow_constructible(T, Args...);
template <class T> static constexpr bool is_nothrow_default_constructible_v = __is_nothrow_constructible(T);
template <class T> static constexpr bool is_nothrow_copy_constructible_v = __is_nothrow_constructible(T, const T &);
template <class T> static constexpr bool is_nothrow_move_constructible_v = __is_nothrow_constructible(T, T &&);
template <class T, class U> static constexpr bool is_nothrow_assignable_v = __is_nothrow_assignable(T, U);
template <class T> static constexpr bool is_nothrow_copy_assignable_v = __is_nothrow_assignable(T &, const T &);
template <class T> static constexpr bool is_nothrow_move_assignable_v = __is_nothrow_assignable(T &, T &&);
template <class T> static constexpr bool is_nothrow_destructible_v = __is_nothrow_destructible(T);
template <class T> static constexpr bool has_virtual_destructor = __has_virtual_destructor(T);
template <class T> static constexpr bool has_unique_object_representations = __has_unique_object_representations(T);
template <class Base, class Derived> static constexpr bool is_base_of_v = __is_base_of(Base, Derived);
template <class From, class To> static constexpr bool is_convertible_v = __is_convertible(From, To);
template <class T, class U> static constexpr bool is_same_v = false;
template <class T> static constexpr bool is_same_v<T, T> = true;

template <class T> static constexpr size_t alignment_of = __alignof(T);

template <class T> static constexpr size_t rank = 0;
template <class T> static constexpr size_t rank<T[]> = rank<T> + 1;

template <class T, size_t N = 0> static constexpr size_t extent = 0;
template <class T> static constexpr size_t extent<T[], 0> = 0;
template <class T, size_t N> static constexpr size_t extent<T[], N> = extent<T, N - 1>;
template <class T, size_t N> static constexpr size_t extent<T[N], 0> = N;
template <class T, size_t I, size_t N> static constexpr size_t extent<T[I], N> = extent<T, N - 1>;


//
// REMOVE_XXX
//

template <class T> struct remove_cv { using type = T; };
template <class T> struct remove_cv<const T> { using type = T; };
template <class T> struct remove_cv<volatile T> { using type = T; };
template <class T> struct remove_cv<const volatile T> { using type = T; };
template <class T> using remove_cv_t = typename remove_cv<T>::type;

template <class T> struct remove_const { using type = T; };
template <class T> struct remove_const<const T> { using type = T; };
template <class T> using remove_const_t = typename remove_const<T>::type;

template <class T> struct remove_volatile { using type = T; };
template <class T> struct remove_volatile<volatile T> { using type = T; };
template <class T> using remove_volatile_t = typename remove_volatile<T>::type;

template <class T> struct remove_reference { using type = T; };
template <class T> struct remove_reference<T &> { using type = T; };
template <class T> struct remove_reference<T &&> { using type = T; };
template <class T> using remove_reference_t = typename remove_reference<T>::type;

template <class T> struct remove_pointer { using type = T; };
template <class T> struct remove_pointer<T *> { using type = T; };
template <class T> struct remove_pointer<T *const> { using type = T; };
template <class T> struct remove_pointer<T *volatile> { using type = T; };
template <class T> struct remove_pointer<T *const volatile> { using type = T; };
template <class T> using remove_pointer_t = typename remove_pointer<T>::type;

template <class T> struct remove_extent { using type = T; };
template <class T> struct remove_extent<T[]> { using type = T; };
template <class T, size_t N> struct remove_extent<T[N]> { using type = T; };
template <class T> using remove_extent_t = typename remove_extent<T>::type;

template <class T> struct remove_all_extents { using type = T; };
template <class T> struct remove_all_extents<T[]> { using type = typename remove_all_extents<T>::type; };
template <class T, size_t N> struct remove_all_extents<T[N]> { using type = typename remove_all_extents<T>::type; };
template <class T> using remove_all_extents_t = typename remove_all_extents<T>::type;

template <class T> struct remove_cvref { using type = remove_cv_t<remove_reference_t<T>>; };
template <class T> using remove_cvref_t = typename remove_cvref<T>::type;

//
// ADD_XXX
//

template <class T> struct add_const { using type = const T; };
template <class T> using add_const_t = typename add_const<T>::type;

template <class T> struct add_volatile { using type = volatile T; };
template <class T> using add_volatile_t = typename add_volatile<T>::type;

template <class T> struct add_cv { using type = const volatile T; };
template <class T> using add_cv_t = typename add_cv<T>::type;

template <class T> struct add_pointer { using type = remove_reference_t<T> *; };
template <class T> using add_pointer_t = typename add_pointer<T>::type;

template <class T> struct add_lvalue_reference { using type = T &; };
template <class T> using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

template <class T> struct add_rvalue_reference { using type = T &&; };
template <class T> using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

//
// TRANSFORM
//

template <class T> struct make_signed { using type = T; };
template <> struct make_signed<char> { using type = signed char; };
template <> struct make_signed<unsigned char> { using type = signed char; };
template <> struct make_signed<unsigned short> { using type = signed short; };
template <> struct make_signed<unsigned int> { using type = signed int; };
template <> struct make_signed<unsigned long> { using type = signed long; };
template <> struct make_signed<unsigned long long> { using type = signed long long; };
template <class T> using make_signed_t = typename make_signed<T>::type;

template <class T> struct make_unsigned { using type = T; };
template <> struct make_unsigned<char> { using type = unsigned char; };
template <> struct make_unsigned<signed char> { using type = unsigned char; };
template <> struct make_unsigned<short> { using type = unsigned short; };
template <> struct make_unsigned<int> { using type = unsigned int; };
template <> struct make_unsigned<long> { using type = unsigned long; };
template <> struct make_unsigned<long long> { using type = unsigned long long; };
template <class T> using make_unsigned_t = typename make_unsigned<T>::type;

//
// CONDITIONS
//

template <bool B, class T = void> struct enable_if {};
template <class T> struct enable_if<true, T> { using type = T; };
template <bool B, class T = void> using enable_if_t = typename enable_if<B, T>::type;

template <bool Q, class A, class B> struct conditional { using type = A; };
template <class A, class B> struct conditional<false, A, B> { using type = B; };
template <bool Q, class A, class B> using conditional_t = typename conditional<Q, A, B>::type;

//
// DECAY
//

template <class T> struct decay {
    using U = remove_reference_t<T>;
    using type = conditional_t<
        is_array_v<U>,
        add_pointer_t<remove_extent_t<U>>,
        conditional_t<
            is_function_v<U>,
            add_pointer_t<U>,
            remove_cv_t<U>
        >
    >;
};
template <class T> using decay_t = typename decay<T>::type;

}
