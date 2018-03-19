
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

#define __USING_PIC

#ifdef __USING_PIC
#define send_eoi pic_send_eoi
#else
#define send_eoi(...) (*(volatile u32 *)0xfee000b0 = 0)
#endif

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

extern void isr_syscall(void);
extern void isr_yield(void);

void raw_set_idt_gate(uint64_t *at, void (*handler)(void),
                      uint64_t flags, uint64_t cs, uint64_t ist) {
    uint64_t h = (uint64_t)handler;
    uint64_t handler_low = h & 0xFFFF;
    uint64_t handler_med = (h >> 16) & 0xFFFF;
    uint64_t handler_high = h >> 32;

    at[0] = handler_low | (cs << 16) | (ist << 32) |
            (flags << 40) | (handler_med << 48);
    at[1] = handler_high;
}

void register_idt_gate(int index, void (*handler)(void),
                       bool user, bool stop_irqs, uint8_t ist) {

    // TODO put these in a header
    uint16_t selector = 8; // kernel CS
    uint8_t rpl = user ? 3 : 0;
    uint8_t type = stop_irqs ? 0xe : 0xf; // interrupt vs trap gates

    uint64_t flags = 0x80 | rpl << 5 | type;

    extern uint64_t idt;
    raw_set_idt_gate(&idt + (2*index), handler, flags, selector, ist);
}

void install_isrs() {
    register_idt_gate(0, isr0, false, false, 0);
    register_idt_gate(1, isr1, false, false, 0);
    register_idt_gate(2, isr2, false, false, 0);
    register_idt_gate(3, isr3, false, false, 0);
    register_idt_gate(4, isr4, false, false, 0);
    register_idt_gate(5, isr5, false, false, 0);
    register_idt_gate(6, isr6, false, false, 0);
    register_idt_gate(7, isr7, false, false, 0);
    register_idt_gate(8, isr8, false, false, 0);
    register_idt_gate(9, isr9, false, false, 0);
    register_idt_gate(10, isr10, false, false, 0);
    register_idt_gate(11, isr11, false, false, 0);
    register_idt_gate(12, isr12, false, false, 0);
    register_idt_gate(13, isr13, false, false, 0);
    register_idt_gate(14, isr14, false, false, 0);
    register_idt_gate(15, isr15, false, false, 0);
    register_idt_gate(16, isr16, false, false, 0);
    register_idt_gate(17, isr17, false, false, 0);
    register_idt_gate(18, isr18, false, false, 0);
    register_idt_gate(19, isr19, false, false, 0);
    register_idt_gate(20, isr20, false, false, 0);
    register_idt_gate(21, isr21, false, false, 0);
    register_idt_gate(22, isr22, false, false, 0);
    register_idt_gate(23, isr23, false, false, 0);
    register_idt_gate(24, isr24, false, false, 0);
    register_idt_gate(25, isr25, false, false, 0);
    register_idt_gate(26, isr26, false, false, 0);
    register_idt_gate(27, isr27, false, false, 0);
    register_idt_gate(28, isr28, false, false, 0);
    register_idt_gate(29, isr29, false, false, 0);
    register_idt_gate(30, isr30, false, false, 0);
    register_idt_gate(31, isr31, false, false, 0);

    register_idt_gate(32, irq0, false, false, 0);
    register_idt_gate(33, irq1, false, false, 0);
    register_idt_gate(34, irq2, false, false, 0);
    register_idt_gate(35, irq3, false, false, 0);
    register_idt_gate(36, irq4, false, false, 0);
    register_idt_gate(37, irq5, false, false, 0);
    register_idt_gate(38, irq6, false, false, 0);
    register_idt_gate(39, irq7, false, false, 0);
    register_idt_gate(40, irq8, false, false, 0);
    register_idt_gate(41, irq9, false, false, 0);
    register_idt_gate(42, irq10, false, false, 0);
    register_idt_gate(43, irq11, false, false, 0);
    register_idt_gate(44, irq12, false, false, 0);
    register_idt_gate(45, irq13, false, false, 0);
    register_idt_gate(46, irq14, false, false, 0);
    register_idt_gate(47, irq15, false, false, 0);

    register_idt_gate(128, isr_syscall, true, false, 0);
    register_idt_gate(129, isr_yield, false, false, 0);
}


extern void uart_irq_handler(interrupt_frame *r);

#define NIRQS 16
void (*irq_handlers[NIRQS])(interrupt_frame *) = {
    timer_handler,      /* IRQ 0 */
    keyboard_handler,   /* IRQ 1 */
    NULL,               /* IRQ 2 */
    uart_irq_handler,   /* IRQ 3 */
    NULL,               /* others */
};

void c_interrupt_shim(interrupt_frame *r) {
    // printf("Interrupt %i\n", r->interrupt_number);

    switch(r->interrupt_number) {
    case 0:  divide_by_zero_exception(r);   break;
//    case 13: gp_exception(r);               break;
    case 14: page_fault(r);                 break;
    case 128: syscall_handler(r);           break;
    default: 
        if (r->interrupt_number < 32) {
            generic_exception(r);
        } else if (r->interrupt_number < 32 + NIRQS) {
            // Dispatch to irq table
            // This allows me to add irq handlers later if needed
            // (including at runtime)
            
            // printf("Dispatching IRQ\n");

            if (irq_handlers[r->interrupt_number - 32]) {
                irq_handlers[r->interrupt_number - 32](r);
            } else {
                other_irq_handler(r);
            }
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
        backtrace_from(r->rbp, 10);
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
    panic("Fault: #GP (General Protection) \nError code: %#04x\n",
            r->error_code);
    backtrace_from(r->rbp, 10);

    printf("Stack dump: (rsp at %#x)\n", r->user_rsp);
    debug_dump((void *)r->user_rsp);
}

#define SYS_YIELD 1

void syscall_handler(interrupt_frame *r) {
    printf("\n");
    printf("Syscall %i at 0x%x\n", r->rax, r->rip);

    extern kthread_t *current_kthread;

    switch (r->rax) {
    case 0:
        printf("%s", r->rbx);
        break;
    case 1:
        printf("killing user thread\n");
        current_kthread->state = THREAD_KILLED;
        asm volatile ("hlt");
        break;
    default:
        // printf("Unhandled!\n");
        break;
    }

}

const char *exception_codes[] = {
    "#DE",
    "#DB",
    "NMI",
    "#BP",
    "#OF",
    "#BR",
    "#UD",
    "#NM",
    "#DF",
    "<none>",
    "#TS",
    "#NP",
    "#SS",
    "#GP",
    "#PF",
    "<reserved>",
    "#MF",
    "#AC",
    "#MC",
    "#XM",
    "#VE",
    "<reserved>",
    "<reserved>",
    "<reserved>",
    "<reserved>",
    "<reserved>",
    "<reserved>",
    "<reserved>",
    "<reserved>",
    "<reserved>",
    "#SX",
    "<reserved>"
};

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
    printf("Fault: %s (%s), error code: %#04x\n",
           exception_codes[r->interrupt_number],
           exception_reasons[r->interrupt_number], r->error_code);
    print_registers(r);

    backtrace_from(r->rbp, 10);

    printf("Stack dump: (rsp at %#x)\n", r->user_rsp);
    debug_dump((void *)r->user_rsp);

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
    uint8_t c = 0;

    while (inb(0x64) & 1) {
        c = inb(0x60);
        printf("Heard scancode %i (%#x)\n", c, c);
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

