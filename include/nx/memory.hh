
#pragma once
#ifndef _NX_MEMORY_HH_
#define _NX_MEMORY_HH_

#include <basic.h>
#include <type_traits.hh>
#include <types.hh>
#include <utility.hh>

namespace nx {

template <typename T>
struct unique_ptr {
    T *ptr;

    unique_ptr() : ptr(new T) {
    }

    unique_ptr(T *p) : ptr(p) {
    }

    unique_ptr(T&& v) : ptr(&v) {
    }

    unique_ptr(unique_ptr const&) = delete;

    unique_ptr(unique_ptr&& u) : ptr(u.ptr) {
        u.ptr = nullptr;
    }

    unique_ptr operator=(unique_ptr const&) = delete;

    unique_ptr operator=(unique_ptr&& u) {
        ptr = u.ptr;
        u.ptr = nullptr;
    }

    ~unique_ptr() {
        if (ptr)
            delete ptr;
    }

    T& operator*() {
        return *ptr;
    }

    T *get() {
        return ptr;
    }
};

/*
template <typename T>
struct unique_ptr<T[]> {
    T *ptr;

    unique_ptr(size_t n) : ptr(new T[n]) {
    }

    unique_ptr(T *p) : ptr(p) {
    }

    ~unique_ptr() {
        delete[] ptr;
    }

    T& operator*() {
        return *ptr;
    }

    T *get() {
        return ptr;
    }

    T& operator[](size_t ix) {
        return ptr[ix];
    }
};
*/

template <typename T>
unique_ptr<T> make_unique() {
    return unique_ptr{ new T };
}

template <typename T>
unique_ptr<T> make_unique(T& value) {
    return unique_ptr<T>{ move(value) };
}

template <typename T>
unique_ptr<T> make_unique(T *ptr) {
    return unique_ptr<T>{ ptr };
}

}

#endif // _NX_MEMORY_HH_

