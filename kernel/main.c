
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/cpu.h>

#include "multiboot.h"

void main(multiboot_info_t *mbdata) {

    initialize();

    printf("Project Nightingale\n");

    __asm__ ( "int $0x00" );
    
    for (;;) { __asm__ ( "hlt" ); }

    abort();

    __builtin_unreachable();

}
