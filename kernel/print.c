 
#include <basic.h>
#include <stdarg.h>
#include <string.h>

#include <cpu/uart.h>
#include <cpu/vga.h>

#include "print.h"

const char *lower_hex_charset = "0123456789abcdef";
const char *upper_hex_charset = "0123456789ABCDEF";

void raw_print(const char *buf, usize len) {
    // vga_write("^", 1); // debug
    vga_write(buf, len);
    // uart_write(COM1, "^", 1); // debug
    uart_write(COM1, buf, len);
}

// TODO: replace this with printf when I add 0-padding.
void debug_print_mem(usize cnt, void *mem_) {
    char *mem = mem_;
    char buf[3];
    buf[2] = ' ';

    for (usize i=0; i<cnt; i++) {
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

// Formats for printf
typedef enum Format {
    NORMAL,
    HEX,
    UPPER_HEX,
    OCTAL,
    BINARY,
    POINTER,
} Format;

typedef struct Format_Info {
    int bytes;
    Format format;
    bool is_signed;
    bool alternate_format;
    bool print_plus;
    bool leave_space;

    struct {
        usize len;
        enum {
            LEFT,
            RIGHT,
        } direction;
        char c;
    } pad;
} Format_Info;

static usize format_int(char *buf, u64 raw_value, Format_Info fmt) {
    int base;
    const char *charset = lower_hex_charset;

    switch (fmt.format) {
    case NORMAL:
        base = 10;
        break;
    case HEX:
        base = 16;
        break;
    case UPPER_HEX:
        base = 16;
        charset = upper_hex_charset;
        break;
    case OCTAL:
        base = 8;
        break;
    case BINARY:
        base = 2;
        break;
    case POINTER:
        base = 16;
        break;
    default: ;
        // report_error
    }

    usize buf_ix = 0;
    char tmp_buf[64];
    memset(tmp_buf, 0, sizeof(tmp_buf));

    if (fmt.is_signed) {
        i64 value;

        switch (fmt.bytes) {
        case 1:
            value = (i64)*(i8 *)&raw_value;
            break;
        case 2:
            value = (i64)*(i16 *)&raw_value;
            break;
        case 4:
            value = (i64)*(i32 *)&raw_value;
            break;
        case 8:
            value = *(i64 *)&raw_value;
        }

        if (value == 0) {
            buf[0] = '0';
            return 1;
        }

        bool negative = false;
        if (value < 0) negative = true;

        while (value != 0) {
            if (negative) {
                tmp_buf[buf_ix++] = charset[-(value % base)];
            } else {
                tmp_buf[buf_ix++] = charset[value % base];
            }
            value /= base;
        }

        bool need_space_for_sign = true;

        while (fmt.pad.len > buf_ix) {
            tmp_buf[buf_ix++] = fmt.pad.c;
            need_space_for_sign = false;
        }

        for (usize i=0; i<buf_ix; i++) {
            if ((negative || fmt.print_plus) && need_space_for_sign) {
                buf[i+1] = tmp_buf[buf_ix - i - 1];
            } else {
                buf[i] = tmp_buf[buf_ix - i - 1];
            }
        }

        if (negative) {
            buf[0] = '-';
            buf_ix++;
        } else if (fmt.print_plus) {
            buf[0] = '+';
            buf_ix++;
        }

        return buf_ix;
    } else { // unsigned
        u64 value;

        switch (fmt.bytes) {
        case 1:
            value = (u64)(u8)raw_value;
            break;
        case 2:
            value = (u64)(u16)raw_value;
            break;
        case 4:
            value = (u64)(u32)raw_value;
            break;
        case 8:
            value = raw_value;
            break;
        }

        if (value == 0) {
            buf[0] = '0';
        }

        while (value != 0) {
            tmp_buf[buf_ix++] = charset[value % base];
            value /= base;
        }

        usize written = buf_ix;

        if (fmt.pad.c != ' ') {
            while (fmt.pad.len > buf_ix) {
                tmp_buf[buf_ix++] = fmt.pad.c;
            }
        }

        if (fmt.alternate_format) {
            int need_extra_for_alternate = 2;

            if (fmt.pad.c == '0') {
                if (fmt.pad.len - written > 0 && fmt.format == OCTAL) {
                    need_extra_for_alternate = 0;
                } else if (fmt.pad.len - written > 1 && (fmt.format == HEX || fmt.format == UPPER_HEX)) {
                    need_extra_for_alternate = 0;
                } else if (fmt.pad.len - written > 0 && (fmt.format == HEX || fmt.format == UPPER_HEX)) {
                    need_extra_for_alternate = 1;
                }
            }

            if (fmt.format == OCTAL) { 
                if (need_extra_for_alternate) {
                    tmp_buf[buf_ix++] = '0';
                } else {
                    tmp_buf[buf_ix] = '0';
                }
            }

            if ((fmt.format == HEX || fmt.format == UPPER_HEX)) { 
                if (need_extra_for_alternate == 2) {
                    tmp_buf[buf_ix++] = 'x';
                    tmp_buf[buf_ix++] = '0';
                } else if (need_extra_for_alternate == 1) {
                    tmp_buf[buf_ix - 1] = 'x';
                    tmp_buf[buf_ix++] = '0';
                } else {
                    tmp_buf[buf_ix - 2] = 'x';
                    tmp_buf[buf_ix - 1] = '0';
                }
            }
        }

        if (fmt.pad.c == ' ') {
            while (fmt.pad.len > buf_ix) {
                tmp_buf[buf_ix++] = fmt.pad.c;
            }
        }

        for (usize i=0; i<buf_ix; i++) {
            buf[i] = tmp_buf[buf_ix - i - 1];
        }

        // print_plus intentionally does nothing for unsigned.
        // This is the correct behavior.

        return buf_ix;
    }
}

#define APPEND_DIGIT(val, d) val *= 10; val += d

usize printf(const char *fmt, ...) {
    char buf[128]; /* TODO: dynamic maximum length */
    memset(buf, 0, 128);
    usize buf_ix = 0;

    va_list args;
    va_start(args, fmt);

    u64 value;

    usize len = strlen(fmt);

    for (usize i=0; i<len; i++) {
        if (fmt[i] == '%') {

            bool do_print_int = false;
            Format_Info format = {
                .bytes = 4,
                .is_signed = false,
                .alternate_format = false,
                .print_plus = false,
                .leave_space = false,
                .format = NORMAL,
                .pad = {
                    .len = 0,
                    .direction = RIGHT,
                    .c = ' ',
                },
            };

next_char: ;
            switch (fmt[++i]) {
            case 'h':
                format.bytes /= 2;
                // if (bytes == 0) report_error
                goto next_char;
            case 'l':
                format.bytes *= 2;
                // if (bytes > 8) report_error
                goto next_char;
            case 'j': // intmax_t (u/isize)
            case 'z': // ssize_t (u/isize)
            case 't': // ptrdiff_t (u/isize)
                format.bytes = 8;
                goto next_char;
            case '#':
                format.alternate_format = true;
                goto next_char;
            case '+':
                format.print_plus = true;
                goto next_char;
            case ' ':
                format.leave_space = true; // unimplemented
                goto next_char;
            case '-':
                if (isdigit(fmt[i+1])) // peek
                    format.pad.direction = LEFT;
                goto next_char;
            case '0':
                if (format.pad.len == 0) {
                    format.pad.c = '0';
                    goto next_char;
                } else {
                    APPEND_DIGIT(format.pad.len, 0);
                    goto next_char;
                }
            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9':
                APPEND_DIGIT(format.pad.len, fmt[i] - '0');
                goto next_char;

            // Format terminals
            case 'd':
            case 'i':
                format.is_signed = true;
                do_print_int = true;
                break;
            case 'u':
                do_print_int = true;
                break;
            case 'x':
                do_print_int = true;
                format.format = HEX;
                break;
            case 'X':
                do_print_int = true;
                format.format = UPPER_HEX;
                break;
            case 'o':
                do_print_int = true;
                format.format = OCTAL;
                break;
            case 'b':
                do_print_int = true;
                format.format = BINARY;
                break;
            case 'p':
                do_print_int = true;
                format.format = POINTER;
                format.bytes = sizeof(void *);
                if (format.alternate_format)
                    format.pad.len = 18;
                else
                    format.pad.len = 16;
                format.pad.c = '0';
                break;
            case 's':
                value = va_arg(args, u64);
                char *str = (char *)value;

                // Break this garbage out in to a function maybe?
                if (format.pad.len) { 
                    usize l = strlen(str);
                    if (format.pad.len > l) {
                        if (format.pad.direction == RIGHT) {
                            for (usize i=0; i<format.pad.len - l; i++) {
                                buf[buf_ix++] = format.pad.c;
                            }
                            while(*str != 0) {
                                buf[buf_ix++] = *str++;
                            }
                        } else if (format.pad.direction == LEFT) {
                            while(*str != 0) {
                                buf[buf_ix++] = *str++;
                            }
                            for (usize i=0; i<format.pad.len - l; i++) {
                                buf[buf_ix++] = format.pad.c;
                            }
                        }
                    } else {
                        // If the string is longer than the pad, it is unaffected.
                        while(*str != 0) {
                            buf[buf_ix++] = *str++;
                        }
                    }
                } else {
                    while(*str != 0) {
                        buf[buf_ix++] = *str++;
                    }
                }
                break;
            case '%':
                buf[buf_ix++] = '%';
                break;
            default: ;
                // report_error
            }

            if (do_print_int) {
                value = va_arg(args, u64);
                buf_ix += format_int(&buf[buf_ix], value, format);
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

