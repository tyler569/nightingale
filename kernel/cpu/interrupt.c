
#include <basic.h>
#include <term/print.h>
#include <panic.h>
#include "interrupt.h"
#include "irq.h"

void divide_by_zero_exception(interrupt_frame *r) {
    panic("Kernel divide by 0\n");
}

void general_protection_exception(interrupt_frame *r) {
    panic("General Protection fault\nError code: 0x%x\n", r->error_code);
}

void panic_exception(interrupt_frame *r) {
    printf("Someone hit the panic interrupt at rip=%x!\n", r->rip);

    disable_irqs();
    halt();
}

void syscall_handler(interrupt_frame *r) {
    printf("Syscall at 0x%x\n", r->rip);
    panic("Syscall not implemented\n");
}

void generic_exception(interrupt_frame *r) {
    printf("Unhandled exception at 0x%x\n", r->rip);
    panic("Exception: 0x%x Error code: 0x%x", r->interrupt_number, r->error_code);
}

