
#include <basic.h>
#include <type_traits.hh>
#include <types.hh>
#include <memory.hh>

namespace nx {

template <typename T>
unique_ptr<T> make_unique() {
    return unique_ptr<T>{new T};
}

template <typename T>
unique_ptr<T> make_unique(T& value) {
    auto u = unique_ptr<T>{};
    *u = value;
}

template <typename T>
unique_ptr<T> make_unique(size_t size) {
    return unique_ptr<T>{new typename remove_extent<T>::type[size]};
}

}

