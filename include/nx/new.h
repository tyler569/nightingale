#pragma once
#ifndef NX_NEW_H
#define NX_NEW_H

#include <stddef.h>

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *ptr) noexcept;
void operator delete[](void *ptr) noexcept;

#endif // NX_NEW_H
