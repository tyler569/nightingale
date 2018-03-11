
#include <basic.h>
#include <string.h>
#include <print.h>
#include <panic.h>
#include <debug.h>
#include <kthread.h>
#include "pic.h"
#include "uart.h"
#include "interrupt.h"
#include "cpu.h"

#ifdef __USING_PIC
#define send_eoi pic_send_eoi
#else
#define send_eoi(...) (*(volatile u32 *)0xfee000b0 = 0)
#endif

void uart_irq_handler(interrupt_frame *r);

#define NIRQS 16
void (*irq_handlers[NIRQS])(interrupt_frame *) = {
    timer_handler,      /* IRQ 0 */
    keyboard_handler,   /* IRQ 1 */
    NULL,               /* IRQ 2 */
    uart_irq_handler,   /* IRQ 3 */
    NULL,               /* others */
};

void c_interrupt_shim(interrupt_frame *r) {
    switch(r->interrupt_number) {
    case 0:  divide_by_zero_exception(r);   break;
    case 13: gp_exception(r);               break;
    case 14: page_fault(r);                 break;
    case 128: syscall_handler(r);           break;
    default: 
        if (r->interrupt_number < 32) {
            generic_exception(r);
        } else if (r->interrupt_number < 32 + NIRQS) {
            // Dispatch to irq table
            // This allows me to add irq handlers later if needed
            if (irq_handlers[r->interrupt_number - 32])
                irq_handlers[r->interrupt_number - 32](r);
            else
                other_irq_handler(r);
        } else {
            panic("Interrupt %i recived I cannot deal with that right now\n", r->interrupt_number);
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
        printf("Fault occured at %#x\n", r->rip);
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

    const char *sentence = "Fault %s %s:%#lx because %s from %s.\n";
    printf(sentence, rw, type, faulting_address, reason, mode);

    printf("Fault occured at %#lx\n", r->rip);
    print_registers(r);
    // backtrace_from_here(10);
    backtrace_from(r->rbp, 10);
    panic();
}

void gp_exception(interrupt_frame *r) {
    printf("\n");
    print_registers(r);
    panic("General Protection fault\nError code: 0x%x\n", r->error_code);
    backtrace_from(r->rbp, 10);
    printf("Stack (rsp currently at %#x):\n", &r);
    debug_dump(r);
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

volatile usize timer_ticks = 0;

void timer_handler(interrupt_frame *r) {
    timer_ticks++;

    // printf("test");

    // Instead of having to scroll (at all) in real video memory, this
    // just gives me a framerate.
    // TODO:
    // Future direction: mark the buffer dirty and only update if needed
    vga_flush();

    // This must be done before the context swap, or it never gets done.
    send_eoi(r->interrupt_number - 32);
    swap_kthread(r, NULL, NULL);
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

