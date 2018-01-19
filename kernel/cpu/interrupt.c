
#include <basic.h>
#include <term/print.h>
#include <panic.h>
#include "interrupt.h"
#include "irq.h"

void divide_by_zero_exception(interrupt_frame *r) {
    printf("\n");
    panic("Kernel divide by 0\n");
}

void page_fault(interrupt_frame *r) {
    printf("\n");

#define PRESENT     0x01
#define WRITE       0x02
#define USERMODE    0x04
#define RESERVED    0x08
#define IFETCH      0x10

    const char *protection = "protection violation";
    const char *not_present = "page not present";
    const char *read = "reading";
    const char *write = "writing";
    const char *user = "user";
    const char *kernel = "kernel";
    const char *reserved = "writing to a reserved field";
    const char *ifetch = "instruction";
    const char *not_ifetch = "data";

    const char *rw, *type, *reason, *mode;

    usize faulting_address;
    asm volatile ( "mov %%cr2, %0" : "=r"(faulting_address) );

    if (r->error_code & PRESENT) {
        reason = protection;
    } else {
        reason = not_present;
    }
    if (r->error_code & WRITE) {
        rw = write;
    } else {
        rw = read;
    }
    if (r->error_code & USERMODE) {
        mode = user;
    } else {
        mode = kernel;
    }
    if (r->error_code & RESERVED) {
        printf("Fault was caused by writing to a reserved field\n");
    }
    if (r->error_code & IFETCH) {
        type = ifetch;
    } else {
        type = not_ifetch;
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
    printf("Syscall at 0x%x\n", r->rip);
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

