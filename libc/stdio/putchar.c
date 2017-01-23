
#include <stdio.h>

#ifdef __is_libk
#include <kernel/tty.h>
#endif

int putchar(int value) {

#ifdef __is_libk
    char c = (char)value;
    terminal_write(&c, sizeof(c));
#else
    //TODO userspace
#endif

    return value;
}
