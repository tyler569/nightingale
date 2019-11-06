
#include <basic.h>
#include <ng/x86/halt.h>

void halt() {
        while (true) {
                asm volatile("cli");
                asm volatile("hlt");
        }
}
