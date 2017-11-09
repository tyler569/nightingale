
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "terminal.h"

const char *lower_hex_charset = "0123456789abcdef";

static size_t format_int32_base(char *buf, int32_t value, int base) {
    if (value == 0) {
        buf[0] = '0';
        return 1;
    }
    size_t buf_ix = 0;
    bool negative = false;
    if (value < 0) negative = true;
    char tmp_buf[32];

    while (value != 0) {
        if (negative) {
            tmp_buf[buf_ix++] = lower_hex_charset[-(value % base)];
        } else {
            tmp_buf[buf_ix++] = lower_hex_charset[value % base];
        }
        value /= base;
    }

    for (size_t i=0; i<buf_ix; i++) {
        if (negative) {
            buf[i+1] = tmp_buf[buf_ix - i - 1];
        } else {
            buf[i] = tmp_buf[buf_ix - i - 1];
        }
    }

    if (negative) buf[0] = '-';

    return negative ? buf_ix + 1 : buf_ix;
}

static size_t format_uint32_base(char *buf, uint32_t value, int base) {
    if (value == 0) {
        buf[0] = '0';
        return 1;
    }

    size_t buf_ix = 0;
    char tmp_buf[32];

    while (value != 0) {
        tmp_buf[buf_ix++] = lower_hex_charset[value % base];
        value /= base;
    }

    for (size_t i=0; i<buf_ix; i++) {
        buf[i] = tmp_buf[buf_ix - i - 1];
    }

    return buf_ix;
}

int printf(const char *fmt, ...) {
    char buf[128]; // 127 char maximum print for now
    memset(buf, 0, 128);
    size_t buf_ix = 0;

    va_list args;
    va_start(args, fmt);

    union {
        char c;
        uint32_t u32;
        int32_t i32;
        uint64_t u64;
        int64_t i64;
    } value;

    size_t len = strlen(fmt);

    for (size_t i=0; i<len; i++) {
        if (fmt[i] == '%') {
            switch (fmt[++i]) {
            case 'd':
            case 'i':
                value.i32 = va_arg(args, int32_t);
                buf_ix += format_int32_base(&buf[buf_ix], value.i32, 10);
                break;
            case 'u':
                value.u32 = va_arg(args, uint32_t);
                buf_ix += format_uint32_base(&buf[buf_ix], value.u32, 10);
                break;
            case 'x':
                value.u32 = va_arg(args, uint32_t);
                buf_ix += format_uint32_base(&buf[buf_ix], value.u32, 16);
                break;
            case '%':
                buf[buf_ix++] = '%';
            }
        } else {
            buf[buf_ix++] = fmt[i];
        }

        if (buf_ix >= 127) break;
    }

    term.write(buf, buf_ix);
    return buf_ix;
}

int kformat(char *buf, const char format, ...) {
    return 0;
    /* To be rewrite attempt */
}

