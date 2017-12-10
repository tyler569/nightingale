
#include <basic.h>
#include <term/print.h>
#include "panic.h"

void panic(const char *fmt, ...) {
    /* TODO: register dump and format arguments */
    printf("KERNEL PANIC\n");
    __asm__ ( "int $1" );
}

