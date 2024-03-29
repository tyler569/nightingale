#pragma once
#ifndef NX_FUNCTIONAL_H
#define NX_FUNCTIONAL_H

#include "memory.h"
#include "type_traits.h"
#include "utility.h"

namespace nx {

template <class T> class function_holder;

template <class R, class... Args> class function_holder<R(Args...)> {
public:
    virtual ~function_holder() = default;

    virtual R operator()(Args &&...args) = 0;
};

template <class T, class R, class... Args>
class function_holder_impl : public function_holder<R(Args...)> {
    T m_function;

public:
    explicit function_holder_impl(T &&func)
        : m_function(forward<T>(func))
    {
    }

    R operator()(Args &&...args) override
    {
        return m_function(forward<Args>(args)...);
    }
};

template <class T> class function;

template <class R, class... Args> class function<R(Args...)> {
    unique_ptr<function_holder<R(Args...)>> m_function;

public:
    function() = default;
    ~function() = default;

    template <class T>
    function(const function &other)
        : m_function(make_unique<T, R, Args...>(
            *static_cast<T *>(other.m_function.get())))
    {
    }

    template <class T> function &operator=(const function &other)
    {
        m_function = make_unique<T, R, Args...>(
            *static_cast<T *>(other.m_function.get()));
    }

    function(function &&) noexcept = default;
    function &operator=(function &&) noexcept = default;

    template <class T>
    function(T &&func)
        : m_function(move(
            make_unique<function_holder_impl<T, R, Args...>>(forward<T>(func))))
    {
    }

    template <class T> function &operator=(T &&func)
    {
        m_function = move(
            make_unique<function_holder_impl<T, R, Args...>>(forward<T>(func)));
        return *this;
    }

    template <class... CallArgs> R operator()(CallArgs &&...args)
    {
        return m_function->operator()(forward<Args>(args)...);
    }
};

// clang-format off

template <class R, class... Args> function(R (*)(Args...)) -> function<R(Args...)>;

/*
 * template<class F> function(F) -> function<see below>;
 * Constraints: &F::operator() is well-formed when treated as an unevaluated
 * operand and either
 *  (16.1) F::operator() is a non-static member function and decltype(&F::operator()) is
 *            either of the form R(G::*)(A...) cv& (opt) noexcept (opt) or of the form
 *            R(*)(G, A...) noexcept (opt) for a type G, or
 *  (16.2) F::operator() is a static member function and decltype(&F::operator()) is
 *            of the form R(*)(A...) noexcept (opt).
 *
 * Remarks: The deduced type is function<R(A...)>.
 */

template <class F> struct __function_signature;
template <class R, class... A> struct __function_signature<R(A...)> { using type = R(A...); };
template <class R, class... A> struct __function_signature<R(A...) noexcept> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...)> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...) const> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...) volatile> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...) const volatile> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...) &> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...) const &> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...) volatile &> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...) const volatile &> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...) noexcept> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...) const noexcept> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...) volatile noexcept> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...) const volatile noexcept> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...) & noexcept> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...) const & noexcept> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...) volatile & noexcept> { using type = R(A...); };
template <class R, class G, class... A> struct __function_signature<R (G::*)(A...) const volatile & noexcept> { using type = R(A...); };

template <class F, class _Signature = __function_signature<decltype(&F::operator())>::type> function(F) -> function<_Signature>;
// clang-format on

}

#endif // NX_FUNCTIONAL_H