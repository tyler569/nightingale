#pragma once
#ifndef NX_NEW_H
#define NX_NEW_H

#include <stddef.h>

[[nodiscard]] void *operator new(size_t size);
[[nodiscard]] void *operator new[](size_t size);
void operator delete(void *ptr) noexcept;
void operator delete[](void *ptr) noexcept;

[[nodiscard]] inline void *operator new(size_t count, void *ptr) noexcept
{
    return ptr;
}
[[nodiscard]] inline void *operator new[](size_t count, void *ptr) noexcept
{
    return ptr;
}

#endif // NX_NEW_H
