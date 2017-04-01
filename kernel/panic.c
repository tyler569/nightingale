
#include <stdio.h>
#include "panic.h"

void panic(const char *fmt, ...) {
    printf("Someday I'll have this call real printf");
    __asm__ ( "int 0x30" );
}

