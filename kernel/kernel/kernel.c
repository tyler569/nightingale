
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/tty.h>
// #include <kernel/serial.h>
#include <kernel/printk.h>

void kmain() {

    for (int i = 10; i >= 0; i--) {
        printf("100 / %i is %i\n", i, 100 / i);
    }
    
    for (;;)
        __asm__ ( "hlt" );

}


