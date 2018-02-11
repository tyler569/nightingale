
#include <basic.h>
#include <string.h>
#include <print.h>
#include <panic.h>
#include <debug.h>
#include <proc.h>

#include "pic.h"
#include "uart.h"
#include "interrupt.h"


void c_interrupt_shim(interrupt_frame *r) {
    switch(r->interrupt_number) {
    case 0:  divide_by_zero_exception(r);   break;
    case 13: gp_exception(r);               break;
    case 14: page_fault(r);                 break;
    case 32: timer_handler(r);              break;
    case 33: keyboard_handler(r);           break;
    case 36: uart_irq_handler(r);           break;
    case 128: syscall_handler(r);           break;
    default: 
        if (r->interrupt_number < 32) {
            generic_exception(r);
        } else if (r->interrupt_number < 48) {
            // Sends end-of-interrupt.
            // Look into OpenBSD's PIC config, they say they have
            // automatic EOI.
            other_irq_handler(r);
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

void gp_exception(interrupt_frame *r) {
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
    printf("Exception: 0x%X (%s) Error code: 0x%x\n",
           r->interrupt_number, exception_reasons[r->interrupt_number], r->error_code);
    print_registers(r);
    panic();
}

/***
 * IRQ handlers
 ***/

u64 timer_ticks = 0;

void proc2_test() {
    while (true) {
        /*
        if ((timer_ticks - 1) % 10 == 0) {
            printf("HI FROM PROC 2 @ %i", timer_ticks);
        }
        */
        printf("*");
        asm volatile ("hlt");
    }
}

char second_stack[4096];
interrupt_frame proc[2] = {
    [0] = {0},
    [1] = {
        .user_rsp = (usize)(&second_stack) + 4096,
        .rip = (usize)&proc2_test,
    } 
};

void timer_handler(interrupt_frame *r) {
    send_end_of_interrupt(r->interrupt_number - 32);

    int running_proc = timer_ticks % 2;
    timer_ticks++;
    int new_proc = timer_ticks % 2;

#if 0 // old handler, moved all to proc.c
    // printf("frame is %i\n", sizeof(interrupt_frame));
    memcpy(&proc[running_proc], r, sizeof(interrupt_frame));
    // printf("Saved proc[%i]:\n", running_proc);
    // print_registers(&proc[running_proc]);

    // proc[new_proc].rsp = proc[running_proc].rsp;
    proc[new_proc].ss = proc[running_proc].ss;
    proc[new_proc].cs = proc[running_proc].cs;
    proc[new_proc].rflags = proc[running_proc].rflags;

    memcpy(r, &proc[new_proc], sizeof(interrupt_frame));

    // printf("Restored proc[%i]:\n", new_proc);
    // print_registers(&proc[new_proc]);
#endif

    do_process_swap(r);

    if (timer_ticks % 1000 == 0) {
        DEBUG_PRINTF("This is tick #%i\n", timer_ticks);
    }

    // send_end_of_interrupt(r->interrupt_number - 32);
    // This must be done before the context swap, or it never gets done.
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
    printf("    rsp: %p    r12: %p\n", r->user_rsp, r->r12);
    printf("    rbp: %p    r13: %p\n", r->rbp, r->r13);
    printf("    rsi: %p    r14: %p\n", r->rsi, r->r14);
    printf("    rdi: %p    r15: %p\n", r->rdi, r->r15);
    printf("    rip: %p    rflags: %p\n", r->rip, r->rflags);

}
