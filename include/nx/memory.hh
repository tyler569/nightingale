
#pragma once
#ifndef _NX_MEMORY_HH_
#define _NX_MEMORY_HH_

#include <basic.h>
#include <type_traits.hh>
#include <types.hh>

namespace nx {

template <typename T>
struct unique_ptr {
    T *ptr;

    unique_ptr() : ptr(new T) {
    }

    unique_ptr(T *p) : ptr(p) {
    }

    ~unique_ptr() {
        delete ptr;
    }

    T& operator*() {
        return *ptr;
    }

    T *get() {
        return ptr;
    }
};

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

template <typename T>
unique_ptr<T> make_unique();

template <typename T>
unique_ptr<T> make_unique(T& value);

template <typename T>
unique_ptr<T> make_unique(size_t size);

}

#endif // _NX_MEMORY_HH_

