#pragma once
#ifndef NX_PRINT_H
#define NX_PRINT_H

#include "string.h"
#include <stdio.h>

namespace nx {

template <class T> void print(const T &arg);
template <class T> void print(const T *arg);

template <class T, class... Args>
void print(string_view fmt, T arg, Args... args)
{
    for (size_t i = 0; i < fmt.len(); i++) {
        if (fmt[i] == '%') {
            if (i + 1 < fmt.len() && fmt[i + 1] == '%') {
                printf("%%");
                i++;
                continue;
            }
            print(arg);
            print(fmt.substr(i + 1), args...);
            return;
        }
        printf("%c", fmt[i]);
    }
}

}

#endif // NG_PRINT_H