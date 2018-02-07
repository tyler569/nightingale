
#include <basic.h>
#include <print.h>
#include <panic.h>
#include <debug.h>

#include "pic.h"
#include "uart.h"
#include "interrupt.h"


void c_interrupt_shim(interrupt_frame *r) {
    switch(r->interrupt_number) {
    case 0:  divide_by_zero_exception(r);   break;
    case 32: timer_handler(r);              break;
    default: 
        if (r->interrupt_number < 32) {
            generic_exception(r);
        } else {
            panic("Interrupt %i thrown and I cannot deal with that right now\n", r->interrupt_number);
        }
        break;
    }
}

/* Exceptions */

void divide_by_zero_exception(interrupt_frame *r) {
    printf("\n");
    print_registers(r);
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

    const char *sentence = "Fault %s %s:%p because %s from %s.\n";
    printf(sentence, rw, type, faulting_address, reason, mode);

    printf("Fault occured at %p\n", r->rip);
    print_registers(r);
    panic();
}

void general_protection_exception(interrupt_frame *r) {
    printf("\n");
    print_registers(r);
    panic("General Protection fault\nError code: 0x%x\n", r->error_code);
}

void panic_exception(interrupt_frame *r) {
    printf("\n");
    printf("Someone hit the panic interrupt at rip=%x!\n", r->rip);
    print_registers(r);
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
    print_registers(r);
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

void keyboard_handler(interrupt_frame *r) {
    u8 c = 0;

    while (inb(0x64) & 1) {
        c = inb(0x60);
        printf("Heard scancode %i\n", c);
    }

    send_end_of_interrupt(r->interrupt_number - 32);
}

void other_irq_handler(struct interrupt_frame *r) {
    printf("Unhandled/unmasked IRQ %i received\n", r->interrupt_number - 32);
    send_end_of_interrupt(r->interrupt_number - 32);
}

/* Utility functions */

void enable_irqs() {
    asm volatile ("sti");
}

void disable_irqs() {
    asm volatile ("cli");
}

void print_registers(interrupt_frame *r) {
    printf("Registers:\n");

    /*
    printf("  rax: %p  rbx: %p  rcx: %p\n", r->rax, r->rbx, r->rcx);
    printf("  rdx: %p  r8 : %p  r9 : %p\n", r->rdx, r->r8, r->r9);
    printf("  r10: %p  r11: %p  r12: %p\n", r->r10, r->r11, r->r12);
    printf("  r13: %p  r14: %p  r15: %p\n", r->r13, r->r14, r->r15);
    printf("  rflags: %p\n", r->rflags);
    */

    printf("    rax: %p    r8 : %p\n", r->rax, r->r8);
    printf("    rbx: %p    r9 : %p\n", r->rbx, r->r9);
    printf("    rcx: %p    r10: %p\n", r->rcx, r->r10);
    printf("    rdx: %p    r11: %p\n", r->rdx, r->r11);
    printf("    rsp: %p    r12: %p\n", r->rsp, r->r12);
    printf("    rbp: %p    r13: %p\n", r->rbp, r->r13);
    printf("    rsi: %p    r14: %p\n", r->rsi, r->r14);
    printf("    rdi: %p    r15: %p\n", r->rdi, r->r15);
    printf("    rip: %p    rflags: %p\n", r->rip, r->rflags);

}
