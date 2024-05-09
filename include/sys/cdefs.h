#pragma once

// Compiler independant attributes
#define __PACKED __attribute__((__packed__))
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
#endif // __cplusplus

// Utils

#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)
#define ARRAY_SIZE(A) (sizeof((A)) / sizeof(*(A)))
#define PTR_ADD(p, off) (void *)(((char *)p) + off)
#define TO_ERROR(R) ((void *)(intptr_t)(R))
#define ERROR(R) ((intptr_t)(R))
#define IS_ERROR(R) \
	({ \
		typeof(R) _r = (R); \
		(intptr_t) _r > -0x1000 && (intptr_t)_r < 0; \
	})

#define MAX(A, B) \
	({ \
		typeof(A) _a = (A); \
		typeof(B) _b = (B); \
		_a < _b ? _b : _a; \
	})

#define MIN(A, B) \
	({ \
		typeof(A) _a = (A); \
		typeof(B) _b = (B); \
		_a < _b ? _a : _b; \
	})

#define ROUND_DOWN(val, place) \
	({ \
		typeof(val) _v = (val); \
		typeof(place) _p = (place); \
		_v & ~(_p - 1); \
	})

#define ROUND_UP(val, place) \
	({ \
		typeof(val) _v = (val); \
		typeof(place) _p = (place); \
		(_v + _p - 1) & ~(_p - 1); \
	})

#define ALIGN_UP ROUND_UP
#define ALIGN_DOWN ROUND_DOWN

#define CONTAINER_OF(ptr, type, member) \
	((type *)((uintptr_t)(ptr)-offsetof(type, member)))

#define _x_UNREACHABLE() __builtin_unreachable()

#define volatile_get(x) (*(volatile typeof(x) *)&(x))
#define volatile_read(x) (*(volatile typeof(x))(x))
#define volatile_write(x, y) ((*(volatile typeof(x))(x)) = (y))

#define USED __attribute__((used, unused))
#define _UNUSED __attribute__((unused))
#define MUST_USE __attribute__((warn_unused_result))
#define PACKED __attribute__((packed))
#define PRINTF_FORMAT(a, b) __attribute__((format(printf, a, b)))
#define PURE __attribute__((pure))
#define WEAK __attribute__((weak))

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define FILE_AND_LINE FILE_BASENAME ":" STRINGIFY(__LINE__)

#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
