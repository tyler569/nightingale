
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/vga.h>

void kmain() {

    printf("Project Nightingale\n");

    
    for (;;) {
        printf("Process 1 *****\n");
        __asm__ ("hlt");
    }

    __builtin_unreachable();

}

void kmain2() {

    printf("Project Threads\n");

    for (;;) {
        printf("Process 2 -----\n");
        __asm ("hlt");
    }

    __builtin_unreachable();
}
