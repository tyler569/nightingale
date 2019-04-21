
#include <ng/basic.h>
#include "halt.h"

void halt() {
    while (true) {
        asm volatile ("cli");
        asm volatile ("hlt");
    }
}

