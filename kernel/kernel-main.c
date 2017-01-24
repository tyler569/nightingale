
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


void kmain() {

    printf("Hello World\nProject Nightingale\n");
    
    for (;;)
        __asm__ ( "hlt" );

}


