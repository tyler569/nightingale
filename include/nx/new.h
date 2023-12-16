#pragma once
#ifndef NX_NEW_H
#define NX_NEW_H

#include <stddef.h>

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *ptr) noexcept;
void operator delete[](void *ptr) noexcept;
inline void *operator new(size_t count, void *ptr) { return ptr; }
inline void *operator new[](size_t count, void *ptr) { return ptr; }

#endif // NX_NEW_H
