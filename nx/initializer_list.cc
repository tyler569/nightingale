
#include <basic.h>
#include <initializer_list.hh>

namespace std {

template <typename T>
constexpr const T *begin(initializer_list<T> i) noexcept {
    return i.begin();
}

template <typename T>
constexpr const T *end(initializer_list<T> i) noexcept {
    return i.end();
}

}

