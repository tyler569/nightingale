
#include <basic.h>
#include <string.h>
#include <print.h>
#include <panic.h>
#include <debug.h>
#include <kthread.h>
#include <arch/x86/pic.h>
#include <arch/x86/uart.h>
#include <arch/x86/interrupt.h>

#ifdef __USING_PIC
#define send_eoi pic_send_eoi
#else
#define send_eoi(...) (*(volatile u32 *)0xfee000b0 = 0)
#endif


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

    if (faulting_address < 0x1000) {
        printf("NULL pointer access!\n");
        printf("Fault occured at %p\n", r->rip);
        print_registers(r);
        panic();
    }

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
    backtrace_from(r->rbp, 10);
    panic();
}

void gp_exception(interrupt_frame *r) {
    printf("\n");
    print_registers(r);
    panic("General Protection fault\nError code: 0x%x\n", r->error_code);
    backtrace_from(r->rbp, 10);
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

void timer_handler(interrupt_frame *r) {
    // This must be done before the context swap, or it never gets done.
    send_eoi(r->interrupt_number - 32);
    timer_ticks++;

    // printf("test");

    // Instead of having to scroll (at all) in real video memory, this
    // just gives me a framerate.
    // TODO:
    // Future direction: mark the buffer dirty and only update if needed
    vga_flush();

    kthread_swap(r, NULL, NULL);
}

void keyboard_handler(interrupt_frame *r) {
    u8 c = 0;

    while (inb(0x64) & 1) {
        c = inb(0x60);
        printf("Heard scancode %i\n", c);
    }

    send_eoi(r->interrupt_number - 32);
}

void other_irq_handler(struct interrupt_frame *r) {
    printf("Unhandled/unmasked IRQ %i received\n", r->interrupt_number - 32);
    send_eoi(r->interrupt_number - 32);
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
    printf("    rax: %16x    r8 : %16x\n", r->rax, r->r8);
    printf("    rbx: %16x    r9 : %16x\n", r->rbx, r->r9);
    printf("    rcx: %16x    r10: %16x\n", r->rcx, r->r10);
    printf("    rdx: %16x    r11: %16x\n", r->rdx, r->r11);
    printf("    rsp: %16x    r12: %16x\n", r->user_rsp, r->r12);
    printf("    rbp: %16x    r13: %16x\n", r->rbp, r->r13);
    printf("    rsi: %16x    r14: %16x\n", r->rsi, r->r14);
    printf("    rdi: %16x    r15: %16x\n", r->rdi, r->r15);
    printf("    rip: %16x    rfl: %16x\n", r->rip, r->rflags);

}
