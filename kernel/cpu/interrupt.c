
#include <basic.h>
#include <term/print.h>
#include <panic.h>
#include <debug.h>

#include "uart.h"
#include "interrupt.h"

void divide_by_zero_exception(interrupt_frame *r) {
    printf("\n");
    panic("Kernel divide by 0\n");
}

void page_fault(interrupt_frame *r) {
    printf("\n");

    const int PRESENT  = 0x01;
    const int WRITE    = 0x02;
    const int USERMODE = 0x04;
    const int RESERVED = 0x08;
    const int IFETCH   = 0x10;

    const char *rw, *type, *reason, *mode;

    usize faulting_address;
    asm volatile ( "mov %%cr2, %0" : "=r"(faulting_address) );

    if (r->error_code & PRESENT) {
        reason = "protection violation";
    } else {
        reason = "page not present";
    }
    if (r->error_code & WRITE) {
        rw = "writing";
    } else {
        rw = "reading";
    }
    if (r->error_code & USERMODE) {
        mode = "user mode";
    } else {
        mode = "kernel mode";
    }
    if (r->error_code & RESERVED) {
        printf("Fault was caused by writing to a reserved field\n");
    }
    if (r->error_code & IFETCH) {
        type = "instruction";
    } else {
        type = "data";
    }

    const char *sentence = "Fault %s %s:%p because %s from %s mode.\n";
    printf(sentence, rw, type, faulting_address, reason, mode);

    printf("Fault occured at %p\n", r->rip);
    panic();
}

void general_protection_exception(interrupt_frame *r) {
    printf("\n");
    panic("General Protection fault\nError code: 0x%x\n", r->error_code);
}

void panic_exception(interrupt_frame *r) {
    printf("\n");
    printf("Someone hit the panic interrupt at rip=%x!\n", r->rip);
    panic();
}

void syscall_handler(interrupt_frame *r) {
    printf("\n");
    printf("Syscall %i at 0x%x\n", r->rax, r->rip);
    panic("Syscall not implemented\n");
}

const char *exception_reasons[] = {
    "Divide by zero", 
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow Trap",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun (Deprecated)",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security Exception",
    "Reserved"
};

void generic_exception(interrupt_frame *r) {
    printf("\n");
    printf("Unhandled exception at 0x%x\n", r->rip);
    printf("Exception: 0x%X (%s) Error code: 0x%x",
           r->interrupt_number, exception_reasons[r->interrupt_number], r->error_code);
    panic();
}

/***
 * IRQ handlers
 ***/

i64 timer_ticks = 0;

void timer_handler(struct interrupt_frame *r) {
    timer_ticks++;

    if (timer_ticks % 1000 == 0) {
        DEBUG_PRINTF("This is tick #%i\n", timer_ticks);
    }

    send_end_of_interrupt(r->interrupt_number - 32);
}

void other_irq_handler(struct interrupt_frame *r) {
    send_end_of_interrupt(r->interrupt_number - 32);
}

