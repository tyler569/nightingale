
#include <term/kprint.h>
#include "panic.h"

void panic(const char *fmt, ...) {
    /* TODO: register dump and format arguments */
    kprintf("KERNEL PANIC\n");
    __asm__ ( "int $1" );
}

