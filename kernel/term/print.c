 
#include <basic.h>
#include <stdarg.h>
#include <string.h>

#include <cpu/uart.h>
#include "terminal.h"
#include "print.h"

const char *lower_hex_charset = "0123456789abcdefghijklmnopqrstuvwxyz";
const char *upper_hex_charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void raw_print(const char *buf, usize len) {
    term_vga.write(buf, len);
    uart_write(COM1, buf, len);
}

void debug_print_mem(i32 cnt, void *mem_) {
    char *mem = mem_;
    char buf[3];
    buf[2] = ' ';

    for (i32 i=0; i<cnt; i++) {
        buf[0] = lower_hex_charset[(mem[i] & 0xf0) >> 4];
        buf[1] = lower_hex_charset[mem[i] & 0x0f];
        raw_print(buf, 3);
    }
    raw_print("\n", 1);
}

void debug_dump(void *mem) {
    printf("128 bytes surrounding address: %p\n", mem);

    for (usize m=(usize)mem-64 & ~0x0f; m < (usize)mem+64; m += 16) {
        printf("%p : ", (void *)m);
        debug_print_mem(16, (void *)m);
    }
}

usize print_ptr(usize ptr, char *buf) {
    for (usize i = 0; i<16; i++) {
        buf[i] = lower_hex_charset[(ptr >> (60 - (4 * i))) & 0xf];
    }
    return 16;
}

static usize format_int32_base(char *buf, i32 value, i32 base, const char *charset) {
    if (value == 0) {
        buf[0] = '0';
        return 1;
    }
    usize buf_ix = 0;
    bool negative = false;
    if (value < 0) negative = true;
    char tmp_buf[32];

    while (value != 0) {
        if (negative) {
            tmp_buf[buf_ix++] = charset[-(value % base)];
        } else {
            tmp_buf[buf_ix++] = charset[value % base];
        }
        value /= base;
    }

    for (usize i=0; i<buf_ix; i++) {
        if (negative) {
            buf[i+1] = tmp_buf[buf_ix - i - 1];
        } else {
            buf[i] = tmp_buf[buf_ix - i - 1];
        }
    }

    if (negative) buf[0] = '-';

    return negative ? buf_ix + 1 : buf_ix;
}

static usize format_uint32_base(char *buf, u32 value, i32 base, const char *charset) {
    if (value == 0) {
        buf[0] = '0';
        return 1;
    }

    usize buf_ix = 0;
    char tmp_buf[32];

    while (value != 0) {
        tmp_buf[buf_ix++] = charset[value % base];
        value /= base;
    }

    for (usize i=0; i<buf_ix; i++) {
        buf[i] = tmp_buf[buf_ix - i - 1];
    }

    return buf_ix;
}

/* TODO: printf_to to a struct abstract_terminal */

usize printf(const char *fmt, ...) {
    char buf[128]; /* TODO: dynamic maximum length */
    memset(buf, 0, 128);
    usize buf_ix = 0;

    va_list args;
    va_start(args, fmt);

    union {
        char c;
        u32 u32;
        i32 i32;
        u64 u64;
        i64 i64;
    } value;

    usize len = strlen(fmt);

    for (usize i=0; i<len; i++) {
        if (fmt[i] == '%') {
            /* TODO: handle more cases and 64 bit with li, ld, etc */
            switch (fmt[++i]) {
            case 'd':
            case 'i':
                value.i32 = va_arg(args, i32);
                buf_ix += format_int32_base(&buf[buf_ix], value.i32, 10, lower_hex_charset);
                break;
            case 'u':
                value.u32 = va_arg(args, u32);
                buf_ix += format_uint32_base(&buf[buf_ix], value.u32, 10, lower_hex_charset);
                break;
            case 'x':
                value.u32 = va_arg(args, u32);
                buf_ix += format_uint32_base(&buf[buf_ix], value.u32, 16, lower_hex_charset);
                break;
            case 'X':
                value.u32 = va_arg(args, u32);
                buf_ix += format_uint32_base(&buf[buf_ix], value.u32, 16, upper_hex_charset);
                break;
            case 'p':
                value.u64 = va_arg(args, u64);
                buf_ix += print_ptr(value.u64, buf + buf_ix);
                break;
            case 's':
                value.u64 = va_arg(args, u64);
                char *str = (char *)value.u64;
                while(*str != 0) {
                    buf[buf_ix++] = *str++;
                }
                break;
            case '%':
                buf[buf_ix++] = '%';
            }
        }
        /*else if (fmt[i] == '\a') {
            printf("%i %i", fmt[i+1]-'a', fmt[i+2]-'a');
            term_vga.color((Color)(fmt[i+1]-'a'), (Color)(fmt[i+2]-'a'));
            // TODO: this applies the colors as we go, but all chars are printed at the end.
            // #LogicError
            i += 2;
        }*/
        else {
            buf[buf_ix++] = fmt[i];
        }

        if (buf_ix >= 127) break;
    }

    raw_print(buf, buf_ix);
    return buf_ix;
}

