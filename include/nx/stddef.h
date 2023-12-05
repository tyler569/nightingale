#pragma once
#ifndef NX_STDDEF_H
#define NX_STDDEF_H

#include <stddef.h>

namespace nx {

using ptrdiff_t = ::ptrdiff_t;
using size_t = ::size_t;
using nullptr_t = decltype(nullptr);
using max_align_t = long double;

}

#endif // NX_STDDEF_H
