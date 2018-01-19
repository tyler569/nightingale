
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef int (*print_callback)(unsigned, const char *);


int device_print(unsigned len, const char *str) {
    // Dummy stub for vga_print
    for (int i=0; i<len; i++) {
        printf("%c", str[i]);
    }
}

// format char '%'
// format terminals:
//  - '%' -> '%'
//  - 'x' -> lower_hex
//  - 'X' -> upper_hex
//  - 'o' -> octal
//  - 'd' -> integer
//  - 'i' -> integer
//  - 'u' -> unsigned
//  - 'c' -> char
//  - 's' -> string
//  - 'p' -> pointer
//
// format nonterminals:
//  - 'l' -> *64

int printf_test(print_callback cb, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int max = strlen(fmt);
    for (int i=0; i<max;) {
        if (fmt[i] == '%') {
            // all the hard work
        } else if (fmt[i] == '&') {
            // color selection
        } else {
            cb(1, &fmt[i]);
            i += 1;
        }
    }

    va_end(args);
}

int main() {
    printf("Simple test:\n");
    printf_test(device_print, "simple simple simple\n");
}
