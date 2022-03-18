#include <basic.h>
#include <ng/cpu.h>
#include <ng/panic.h>
#include <ng/syscalls.h>
#include <stdnoreturn.h>
#include <x86/interrupt.h>

sysret sys_haltvm(int exit_code)
{
    outb(0x501, exit_code);
    printf("Stopping the VM failed\n");
    return 1;
}

noreturn void halt()
{
    while (true) {
        disable_irqs();
        asm volatile("hlt");
    }
}
