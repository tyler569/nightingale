
#pragma once
#ifndef NG_UBSAN_H
#define NG_UBSAN_H

#include <stdint.h>

struct source_location {
        const char *file_name;
        uint32_t line;
        uint32_t column;
};

struct typedesc {
        uint16_t type_kind;
        uint16_t type_info;
        char type_name[1];
};

struct ubsan_overflow_info {
        struct source_location loc;
        struct typedesc *type;
};

struct ubsan_invalid_value_info {
        struct source_location loc;
        struct typedesc *type;
};

struct ubsan_shift_oob_info {
        struct source_location loc;
        struct typedesc *lhs_type;
        struct typedesc *rhs_type;
};

#endif // NG_UBSAN_H

