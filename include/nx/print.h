#pragma once

#include "nx.h"
#include "string.h"
#include "string_view.h"
#include <stdio.h>

namespace nx {

template <class T> void print(T *const &arg) { printf("%p", arg); }

template <class T> void print(const T &arg);
template <class T> void print(const T *arg);

template <class T, class... Args>
void print(std::string_view fmt, T arg, Args... args)
{
    auto pos = fmt.find('%');
    if (pos == std::string_view::npos) {
        __raw_print(nullptr, fmt.c_str(), fmt.len());
    } else if (pos > 0) {
        __raw_print(nullptr, fmt.c_str(), pos);
        print(fmt.substr(pos), arg, args...);
    } else {
        print(arg);
        print(fmt.substr(1), args...);
    };
}

}
