
#ifndef NG_LIBC_BASIC_H
#define NG_LIBC_BASIC_H

#if __STDC_VERSION__ > 201112L
#define static_assert _Static_assert
#endif

#ifdef __x86_64__
typedef signed long ssize_t;
static_assert(sizeof(ssize_t) == sizeof(void*), "long must be pointer width");
#endif
// other platforms

#endif

