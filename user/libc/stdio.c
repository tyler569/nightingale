
#include <basic.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#define NDEBUG
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include "stdio.h"

const char *lower_hex_charset = "0123456789abcdef";
const char *upper_hex_charset = "0123456789ABCDEF";

/*inline char v_to_ch(char v) {
    if (v >= 0 && v < 10) {
        return v + '0';
    } else if (v >= 10 && v < 16) {
        return v + 'a';
    } else {
        return 'X';
    }
}*/

ssize_t raw_print(const char *buf, size_t len) {
    if (write(stdout, buf, len) == -1) {
        perror("write()");
    }
    return len;
}

ssize_t puts(const char *str) {
    ssize_t len = raw_print(str, strlen(str));
    len += raw_print("\n", 1);
    return len;
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
        size_t len;
        enum {
            LEFT,
            RIGHT,
        } direction;
        char c;
    } pad;
} Format_Info;

static size_t format_int(char *buf, uint64_t raw_value, Format_Info fmt) {
    int base;
    const char *charset = lower_hex_charset;

    if (raw_value == 0 && fmt.format == POINTER) {
        const char* null_print = "(NULL)";
        int len = strlen(null_print);
        memcpy(buf, null_print, len);
        return len;
    }

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

    size_t buf_ix = 0;
    char tmp_buf[64];
    memset(tmp_buf, 0, sizeof(tmp_buf));

    if (fmt.is_signed) {
        int64_t value;

        switch (fmt.bytes) {
        case 1:
            value = (int64_t)*(int8_t*)&raw_value;
            break;
        case 2:
            value = (int64_t)*(int16_t*)&raw_value;
            break;
        case 4:
            value = (int64_t)*(int32_t*)&raw_value;
            break;
        case 8:
            value = *(int64_t*)&raw_value;
        }

        if (value == 0) {
            tmp_buf[0] = '0';
            buf_ix++;
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

        for (size_t i=0; i<buf_ix; i++) {
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
        uint64_t value;

        switch (fmt.bytes) {
        case 1:
            value = (uint64_t)(uint8_t)raw_value;
            break;
        case 2:
            value = (uint64_t)(uint16_t)raw_value;
            break;
        case 4:
            value = (uint64_t)(uint32_t)raw_value;
            break;
        case 8:
            value = raw_value;
            break;
        }

        if (value == 0) {
            tmp_buf[0] = '0';
            buf_ix++;
        }

        while (value != 0) {
            tmp_buf[buf_ix++] = charset[value % base];
            value /= base;
        }

        size_t written = buf_ix;

        if (fmt.pad.c == '0') {
            while (fmt.pad.len > buf_ix) {
                tmp_buf[buf_ix++] = fmt.pad.c;
            }
        }

        if (fmt.alternate_format) {
            int need_extra_for_alternate = 2;

            if (fmt.pad.c == '0') {
                if (fmt.pad.len - written > 0 && fmt.format == OCTAL) {
                    need_extra_for_alternate = 0;
                } else if (fmt.pad.len - written > 1 &&
                           (fmt.format == HEX ||
                            fmt.format == UPPER_HEX ||
                            fmt.format == POINTER)) {
                    need_extra_for_alternate = 0;
                } else if (fmt.pad.len - written > 0 &&
                           (fmt.format == HEX ||
                            fmt.format == UPPER_HEX ||
                            fmt.format == POINTER)) {
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

            if (fmt.format == HEX ||
                fmt.format == UPPER_HEX ||
                fmt.format == POINTER) {

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

        for (size_t i=0; i<buf_ix; i++) {
            buf[i] = tmp_buf[buf_ix - i - 1];
        }

        // print_plus intentionally does nothing for unsigned.
        // This is the correct behavior.

        return buf_ix;
    }
}

#define APPEND_DIGIT(val, d) val *= 10; val += d

ssize_t printf(const char* fmt, ...) {
    char buf[512]; /* TODO: dynamic maximum length */
    // memset(buf, 0, 512);
    size_t buf_ix = 0;

    va_list args;
    va_start(args, fmt);

    uint64_t value;

    size_t len = strlen(fmt);

    for (size_t i=0; i<len; i++) {
        if (fmt[i] == '%') {

            bool do_print_int = false;
            bool constrain_string_len = false;
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

            int l_count = 0;
            int h_count = 0;

next_char: ;
            switch (fmt[++i]) {
            case 'h':
                h_count += 1;
                if (l_count) { assert(false, "can't have h and l in printf"); }
                if (h_count == 1) { format.bytes = sizeof(short); }
                else if (h_count == 2) { format.bytes = sizeof(char); }
                else { assert(false, "too many hs in printf"); }
                goto next_char;
            case 'l':
                l_count += 1;
                if (h_count) { assert(false, "can't have l and h in printf"); }
                if (l_count == 1) { format.bytes = sizeof(long); }
                else if (l_count == 2) { format.bytes = sizeof(long long); }
                else { assert(false, "too many ls in printf"); }
                goto next_char;
            case 'j': // intmax_t
            case 'z': // ssize_t
            case 't': // ptrdiff_t
                format.bytes = sizeof(void*);
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
            case '.':
                constrain_string_len = true;
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
                format.bytes = sizeof(void*);
                format.alternate_format = true;
                format.pad.len = sizeof(void*) * 2 + 2;
                format.pad.c = '0';
                break;
            case 'c':
                value = va_arg(args, int);
                buf[buf_ix++] = value;
                break;
            case 's':
                value = (uint64_t)(uintptr_t)va_arg(args, char*);
                char *str = (char *)(uintptr_t)value;

                // Break this garbage out in to a function maybe?
                if (format.pad.len || constrain_string_len) { 
                    size_t l = strlen(str);
                    if (format.pad.len > l && !constrain_string_len) {
                        if (format.pad.direction == RIGHT) {
                            for (size_t i=0; i<format.pad.len - l; i++) {
                                buf[buf_ix++] = format.pad.c;
                            }
                            while(*str != 0) {
                                buf[buf_ix++] = *str++;
                            }
                        } else if (format.pad.direction == LEFT) {
                            while(*str != 0) {
                                buf[buf_ix++] = *str++;
                            }
                            for (size_t i=0; i<format.pad.len - l; i++) {
                                buf[buf_ix++] = format.pad.c;
                            }
                        }
                    } else if (format.pad.len < l && constrain_string_len) {
                        for (size_t i=0; i<format.pad.len; i++) {
                            if (*str == 0)  break;
                            buf[buf_ix++] = *str++;
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
                if (format.bytes == 8)
                    value = va_arg(args, uint64_t);
                else
                    value = va_arg(args, uintptr_t);
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

        if (buf_ix >= 511) break;
    }

    raw_print(buf, buf_ix);
    return buf_ix;
}

#if 0 // old and shit version

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
        size_t len;
        enum {
            LEFT,
            RIGHT,
        } direction;
        char c;
    } pad;
} Format_Info;

static size_t format_int(char *buf, uint64_t raw_value, Format_Info fmt) {
    int base;
    const char *charset = lower_hex_charset;
    // if (lower_hex_charset == NULL) {
    //     raw_print("L", 1);
    // }
    // if (charset == NULL) {
    //     raw_print("X", 1);
    // }

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

    size_t buf_ix = 0;
    char tmp_buf[64];
    memset(tmp_buf, 0, sizeof(tmp_buf));

    if (fmt.is_signed) {
        int64_t value;

        switch (fmt.bytes) {
        case 1:
            value = (int64_t)*(int8_t *)&raw_value;
            break;
        case 2:
            value = (int64_t)*(int16_t *)&raw_value;
            break;
        case 4:
            value = (int64_t)*(int32_t *)&raw_value;
            break;
        case 8:
            value = *(int64_t *)&raw_value;
        }

        if (value == 0) {
            tmp_buf[0] = '0';
            buf_ix++;
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

        for (size_t i=0; i<buf_ix; i++) {
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
        uint64_t value;

        switch (fmt.bytes) {
        case 1:
            value = (uint64_t)(uint8_t)raw_value;
            break;
        case 2:
            value = (uint64_t)(uint16_t)raw_value;
            break;
        case 4:
            value = (uint64_t)(uint32_t)raw_value;
            break;
        case 8:
            value = raw_value;
            break;
        }

        if (value == 0) {
            tmp_buf[0] = '0';
            buf_ix++;
        }

        while (value != 0) {
            tmp_buf[buf_ix++] = charset[value % base];
            value /= base;
        }

        size_t written = buf_ix;

        if (fmt.pad.c == '0') {
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

        for (size_t i=0; i<buf_ix; i++) {
            buf[i] = tmp_buf[buf_ix - i - 1];
        }

        // print_plus intentionally does nothing for unsigned.
        // This is the correct behavior.

        return buf_ix;
    }
}

#define APPEND_DIGIT(val, d) val *= 10; val += d

ssize_t printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    // if (lower_hex_charset == NULL) {
    //     raw_print("M", 1);
    // }
    char buf[512]; /* TODO: dynamic maximum length */
    // memset(buf, 0, 512);
    size_t buf_ix = 0;

    uint64_t value;

    size_t len = strlen(fmt);

    for (size_t i=0; i<len; i++) {
        if (fmt[i] == '%') {

            bool do_print_int = false;
            bool constrain_string_len = false;
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
            case '.':
                constrain_string_len = true;
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
            case 'c':
                value = va_arg(args, uint64_t);
                buf[buf_ix++] = value;
                // raw_print("c", 1);
                break;
            case 's':
                value = va_arg(args, uint64_t);
                char *str = (char *)value;

                // Break this garbage out in to a function maybe?
                if (format.pad.len || constrain_string_len) { 
                    size_t l = strlen(str);
                    if (format.pad.len > l && !constrain_string_len) {
                        if (format.pad.direction == RIGHT) {
                            for (size_t i=0; i<format.pad.len - l; i++) {
                                buf[buf_ix++] = format.pad.c;
                            }
                            while(*str != 0) {
                                buf[buf_ix++] = *str++;
                            }
                        } else if (format.pad.direction == LEFT) {
                            while(*str != 0) {
                                buf[buf_ix++] = *str++;
                            }
                            for (size_t i=0; i<format.pad.len - l; i++) {
                                buf[buf_ix++] = format.pad.c;
                            }
                        }
                    } else if (format.pad.len < l && constrain_string_len) {
                        for (size_t i=0; i<format.pad.len; i++) {
                            if (*str == 0)  break;
                            buf[buf_ix++] = *str++;
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
                value = va_arg(args, uint64_t);
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

        if (buf_ix >= 511) break;
    }

    raw_print(buf, buf_ix);
    return buf_ix;
}
#endif

char getchar(void) {
    char c;
    read(stdin, &c, 1);
    return c;
}
