
#ifndef NG_LIBC_BASIC_H
#define NG_LIBC_BASIC_H

#if __STDC_VERSION__ > 201112L
#define static_assert _Static_assert
#endif

// #if defined(__x86_64__) || defined(__i386__)
typedef signed long ssize_t;
static_assert(sizeof(ssize_t) == sizeof(void*), "long must be pointer width");
// #else
// # error unsupported architecture
// #endif

#ifdef __GNUC__
#define __unreachable __builtin_unreachable()
#else
#error "Support non-GNUC attributes first"
#endif

#endif

