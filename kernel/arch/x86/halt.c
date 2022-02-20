#include <basic.h>
#include <ng/cpu.h>
#include <stdnoreturn.h>
#include <x86/interrupt.h>

noreturn void halt() {
    while (true) {
        disable_irqs();
        asm volatile ("hlt");
    }
}
