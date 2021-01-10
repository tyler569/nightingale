#include <assert.h>
#include <byteswap.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

ssize_t scan(void *buffer, void **end, const char *format, ...);
void *blit(void *buffer, size_t len, const char *format, ...);

/*
 * blit(buf, len, "%i %i %i", 3, 4, 6);
 * scan(buf, NULL, "%i %i %i", &a, &b, &c);
 */

// IP header: "%c %c %bhu %bhu %bhu %bu %bu"
// for (i = 5; i < ihl; i++) "%bhu", "variable"

// (i is implied)
//  8: l lu
//  4: i u
//  2: h hu
//  1: c cu

//  count"string": s

// big endian: b

// "pid\0tid\0ppid\0comm\0\n"
// "%i%i%i%s\n"
// {3, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 's', 'h'},

// -- terminals
// i
// u
// s
// 
// -- optionals
// h
// l
// c
// 
// -- modifiers
// b

// #define error do { fprintf(stdout, "error\n"); state = OUTSIDE; } while (0)
#define unreachable assert("unreachable" && 0)
#define error assert("error" && 0)

void *blit_int(void *buffer, size_t *len, long l, bool big_endian, int bytes) {
    if (bytes == 1) {
        *(char *)buffer = (char)l;
    } else if (bytes == 2) {
        if (big_endian) {
            *(short *)buffer = bswap_16((short)l);
        } else {
            *(short *)buffer = (short)l;
        }
    } else if (bytes == 4) {
        if (big_endian) {
            *(int *)buffer = bswap_32((int)l);
        } else {
            *(int *)buffer = (int)l;
        }
    } else if (bytes == 8) {
        if (big_endian) {
            *(long *)buffer = bswap_64((long)l);
        } else {
            *(long *)buffer = (long)l;
        }
    }

    *len -= bytes;
    return buffer + bytes;
}

void *blit_unsigned(void *buffer, size_t *len, long l, bool big_endian, int bytes) {
    if (bytes == 1) {
        *(unsigned char *)buffer = (unsigned char)l;
    } else if (bytes == 2) {
        if (big_endian) {
            *(unsigned short *)buffer = bswap_16((unsigned short)l);
        } else {
            *(unsigned short *)buffer = (unsigned short)l;
        }
    } else if (bytes == 4) {
        if (big_endian) {
            *(unsigned int *)buffer = bswap_32((unsigned int)l);
        } else {
            *(unsigned int *)buffer = (unsigned int)l;
        }
    } else if (bytes == 8) {
        if (big_endian) {
            *(unsigned long *)buffer = bswap_64((unsigned long)l);
        } else {
            *(unsigned long *)buffer = (unsigned long)l;
        }
    }

    *len -= bytes;
    return buffer + bytes;
}

void *blit_string(void *buffer, size_t *len, const char *s) {
    size_t slen = strlen(s);
    buffer = blit_int(buffer, len, slen, false, 4);
    strncpy(buffer, s, *len);
    *len -= slen;
}

void *blit(void *buffer, size_t len, const char *format, ...) {
    va_list args;
    va_start(args, format);

    size_t i = 0;
    size_t format_index = 0;
    while (i < len) {
        bool is_signed = true;
        bool is_string = false;
        bool big_endian = false;
        int bytes = 4;
        char c;

        enum state {
            OUTSIDE,
            NONTERMINAL,
            OPTIONAL,
            TERMINAL,
        } state = OUTSIDE;

consume_another:
        c = format[format_index];

        printf("c: '%c' state: %i\n", c, state);

        if (state == OUTSIDE && c != 0 && c != '%') {
            format_index++;
            continue;
        }

        switch (c) {
        case 'i':
            state = TERMINAL;
            break;
        case 'u':
            is_signed = false;
            state = TERMINAL;
            break;
        case 's':
            is_string = true;
            state = TERMINAL;
            break;
        case 'h':
            bytes = 2;
            state = OPTIONAL;
            break;
        case 'l':
            bytes = 8;
            state = OPTIONAL;
            break;
        case 'c':
            bytes = 1;
            state = OPTIONAL;
            break;
        case 'b':
            big_endian = true;
            state = NONTERMINAL;
            break;
        case '%':
            if (state == OUTSIDE) state = NONTERMINAL;
            else if (state == OPTIONAL) {
                format_index--;
                state = TERMINAL;
            }
            else if (state == TERMINAL) error;
            else if (state == NONTERMINAL) error;
            break;
        case 0:
            if (state == OPTIONAL) state = TERMINAL;
            else if (state == TERMINAL) unreachable;
            else if (state == NONTERMINAL) error;
            break;
        default:
            if (state == OPTIONAL) {
                format_index--;
                state = TERMINAL;
            }
            else if (state == TERMINAL) unreachable;
            else if (state == NONTERMINAL) error;
        }
        format_index++;
        if (state == TERMINAL) {
            if (is_string) {
                const char *s = va_arg(args, const char *);
                buffer = blit_string(buffer, &len, s);
            } else if (is_signed && bytes < 8) {
                int l = va_arg(args, int);
                buffer = blit_int(buffer, &len, l, big_endian, bytes);
            } else if (is_signed) {
                long l = va_arg(args, long);
                buffer = blit_int(buffer, &len, l, big_endian, bytes);
            } else if (!is_signed && bytes < 8) {
                unsigned int l = va_arg(args, unsigned int);
                buffer = blit_unsigned(buffer, &len, l, big_endian, bytes);
            } else if (!is_signed) {
                unsigned long l = va_arg(args, unsigned long);
                buffer = blit_unsigned(buffer, &len, l, big_endian, bytes);
            }
            if (c == 0) break;
            state = OUTSIDE;
        } else {
            if (c == 0) break;
            goto consume_another;
        }
    }

    va_end(args);
}

int main() {
    char buffer[128] = {0};

    blit(buffer, 128, "%i%bi%s", 1, 2, "Hello");

    for (int i=0; i<20; i++) {
        printf("%02hhx ", buffer[i]);
    }
    printf("\n");
}
