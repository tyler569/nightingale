
#include <ng/basic.h>
#include <arch/x86/halt.h>

void halt() {
        while (true) {
                asm volatile("cli");
                asm volatile("hlt");
        }
}
