
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef int i32;
typedef long int i64;
typedef unsigned int u32;
typedef unsigned long int u64;
typedef _Bool bool;
enum {
    false = 0,
    true = 1,
};

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
//  - 'l' -> *64 (shorter isn't needed, since implicit upconversion to 32 bit)
//
//  - ' ' -> space fill
//  - '0' -> zero fill
//  - num -> how much fill

int printf_test(print_callback cb, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int max = strlen(fmt);
    for (int i=0; i<max;) {

        bool pad = false;
        int padding_len = 0;
        char pad_char = '\0';

        bool is_long = false;

        i64 ivalue;
        u64 uvalue;

        if (fmt[i] == '%') {
            while (true) {
                switch (fmt[i]) {
                case 'l':
                    is_long = true;
                    i += 1;
                    break;
                case 'i':
                    if (is_long)
                        ivalue = va_arg(args, i64);
                    else
                        ivalue = va_arg(args, i32);
                    // do format value
                    printf("%li", ivalue); // cheat
                    i += 1;
                    goto done;
                case 'u':
                    if (is_long)
                        uvalue = va_arg(args, u64);
                    else
                        uvalue = va_arg(args, u32);
                    // do format value
                    printf("%lu", uvalue); // cheat
                    i += 1;
                    goto done;
                }
            }
        } else if (fmt[i] == '&') {
            // color selection
            i += 1;
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

    printf_test(device_print, "integer: %i\n", 102);
}
