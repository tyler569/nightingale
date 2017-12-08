
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "terminal.h"
#include "print.h"

const char *lower_hex_charset = "0123456789abcdefghijklmnopqrstuvwxyz";

void debug_print_mem(int cnt, void *mem_) {
    char *mem = mem_;
    char buf[3];
    buf[2] = ' ';

    for (int i=0; i<cnt; i++) {
        buf[0] = lower_hex_charset[(mem[i] & 0xf0) >> 4];
        buf[1] = lower_hex_charset[mem[i] & 0x0f];
        term_vga.write(buf, 3);
    }
    term_vga.write("\n", 1);
}

void debug_dump(void *mem) {
    printf("128 bytes surrounding address: %p\n", mem);

    for (uintptr_t m=(uintptr_t)mem-64 & ~0x0f; m < (uintptr_t)mem+64; m += 16) {
        printf("%p : ", (void *)m);
        debug_print_mem(16, (void *)m);
    }
}

size_t print_ptr(uintptr_t ptr, char *buf) {
    for (size_t i = 0; i<16; i++) {
        buf[i] = lower_hex_charset[(ptr >> (60 - (4 * i))) & 0xf];
    }
    return 16;
}

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

/* TODO: printf_to to a struct abstract_terminal */

int printf(const char *fmt, ...) {
    char buf[128]; /* TODO: dynamic maximum length */
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
            /* TODO: handle more cases and 64 bit with li, ld, etc */
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
            case 'p':
                value.u64 = va_arg(args, uint64_t);
                buf_ix += print_ptr(value.u64, buf + buf_ix);
                break;
            case '%':
                buf[buf_ix++] = '%';
            }
        } else {
            buf[buf_ix++] = fmt[i];
        }

        if (buf_ix >= 127) break;
    }

    term_vga.write(buf, buf_ix);
    return buf_ix;
}

