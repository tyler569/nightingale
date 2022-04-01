#pragma once

// Compiler independant attributes
#define __PACKED __attribute__((__packed__))
#define __NORETURN __attribute__((__noreturn__))
#define __USED __attribute__((__used__, __unused__))
#define __MUST_EMIT __USED
#define __MAYBE_UNUSED __attribute__((__unused__))
#define __ALIGN(X) __attribute__((__aligned__(X)))
#define __NOINLINE __attribute__((__noinline__))
#define __RETURNS_TWICE __attribute__((__returns_twice__))
#define __MUST_USE __attribute__((__warn_unused_result__))
#define __WEAK __attribute__((__weak__))
#define __PRINTF(index, firstarg) \
    __attribute__((__format__(__printf__, index, firstarg)))
#define __MALLOC(...) __attribute__((__malloc__, __alloc_size__(__VA_ARGS__)))

#ifdef __cplusplus
#define BEGIN_DECLS extern "C" {
#define END_DECLS }
#define restrict
#else
#define BEGIN_DECLS
#define END_DECLS
#define static_assert(A) _Static_assert(A, #A)
#endif // __cplusplus
