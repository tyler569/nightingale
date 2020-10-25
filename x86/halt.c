
#include <basic.h>
#include <ng/x86/interrupt.h>
#include <ng/x86/halt.h>

void halt() {
        while (true) {
                disable_irqs();
                asm volatile("hlt");
        }
}
