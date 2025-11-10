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
#define ARRAY_LEN(A) (sizeof((A)) / sizeof(*(A)))
#define PTR_ADD(p, off) (void *)(((char *)p) + off)
#define TO_ERROR(R) ((void *)(intptr_t)(R))
#define ERROR(R) ((intptr_t)(R))
#define IS_ERROR(R) \
	({ \
		__auto_type _r = (R); \
		(intptr_t)_r > -0x1000 && (intptr_t)_r < 0; \
	})

#define MAX(A, B) \
	({ \
		__auto_type _a = (A); \
		__auto_type _b = (B); \
		_a < _b ? _b : _a; \
	})

#define MIN(A, B) \
	({ \
		__auto_type _a = (A); \
		__auto_type _b = (B); \
		_a < _b ? _a : _b; \
	})

#define ROUND_DOWN(val, place) \
	({ \
		__auto_type _v = (val); \
		__auto_type _p = (place); \
		_v & ~(_p - 1); \
	})

#define ROUND_UP(val, place) \
	({ \
		__auto_type _v = (val); \
		__auto_type _p = (place); \
		(_v + _p - 1) & ~(_p - 1); \
	})
