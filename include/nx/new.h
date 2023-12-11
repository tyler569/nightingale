#pragma once

#include "nx.h"
#include <stddef.h>

#ifdef __nx_is_std

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *ptr) noexcept;
void operator delete[](void *ptr) noexcept;

#endif // __nx_is_std