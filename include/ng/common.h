#pragma once

#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)
#define ARRAY_LEN(A) (sizeof((A)) / sizeof(*(A)))
#define PTR_ADD(p, off) (void *)(((char *)p) + off)
#define TO_ERROR(R) ((void *)(intptr_t)(R))
#define ERROR(R) ((intptr_t)(R))
#define IS_ERROR(R) \
    ({ \
        __auto_type _r = (R); \
        (intptr_t) _r > -0x1000 && (intptr_t)_r < 0; \
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
