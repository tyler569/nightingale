
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifdef __GNUC__
# define PRINTF __attribute__((format (printf, 2, 3)))
#endif

void print_callback(const char* buf, int len) {
    printf("%.*s", len, buf);
}

typedef void (*cb)(const char*, int);

// PRINTF
long printf_to(cb emit, const char* fmt, ...) {
    int pad_width = 0;
    int pad_char = 0;
    int byte_width = sizeof(void*);
    int do_argument = 0;

    while (*fmt) {
        switch (*fmt) {
        case '%':
            if (do_argument) {
                emit("%", 1);
            }
            do_argument = !do_argument;
            break;
        default:
            if (do_argument) {
                emit("TODO", 4);
                do_argument = 0;
            } else {
                emit(fmt, 1);
            }
            break;
        }

        fmt += 1;
    }
}

/*
#define SUBSYSTEM SUBSYSTEM_THREAD
#define SUBSYSTEM_NAME "thread"
#define DEBUGLEVEL_OVERRIDE LOG_INFO
#define kprintf(...) kprintf_s(SUBSYSTEM, SUBSYSTEM_NAME ": " __VA_ARGS__);

kprintf(LOG_WARN, "foo bar baz %i\n", foo);
// thread: foo bar baz 66
kprintf(LOG_INFO, "allocated at %p\n", bar);
// thread: allocated at 0xwhatever
kprintf(LOG_DEBUG, "resolved 6 to 6");
// nothing because the debug level isn't active

kprintf should also log to a buffer or file somewhere
*/

int main() {
    printf_to(print_callback, "foo bar %% %i foo bar\n");
}

