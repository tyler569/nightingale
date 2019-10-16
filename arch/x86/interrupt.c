
#include <ng/basic.h>
#include <ng/debug.h>
#include <ng/panic.h>
#include <ng/print.h>
#include <ng/string.h>
// #include <kthread.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include "uart.h"
#include "interrupt.h"
#include "pic.h"
#include "pit.h"
// #include "cpu.h"

#define USING_PIC 1

#ifdef USING_PIC
#define send_eoi pic_send_eoi
#else
#define send_eoi(...) (*(volatile u32 *)0xfee000b0 = 0)
#endif

// Stack dumps are not particularly helpful in the general case. This could be
// a runtime option though, it's a good candidate for that system when it
// happens.
#define DO_STACK_DUMP 0

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
extern void isr_panic(void);

// Change all of this to uintptr_t?
#if X86_64
void raw_set_idt_gate(uint64_t *idt, int index, void (*handler)(void),
                      uint64_t flags, uint64_t cs, uint64_t ist) {
        uint64_t *at = idt + index * 2;

        uint64_t h = (uint64_t)handler;
        uint64_t handler_low = h & 0xFFFF;
        uint64_t handler_med = (h >> 16) & 0xFFFF;
        uint64_t handler_high = h >> 32;

        at[0] = handler_low | (cs << 16) | (ist << 32) | (flags << 40) |
                (handler_med << 48);
        at[1] = handler_high;
}
#elif I686
void raw_set_idt_gate(uint64_t *idt, int index, void (*handler)(void),
                      uint64_t flags, uint64_t cs, uint64_t ist) {
        uint64_t *at = idt + index;

        uint32_t h = (uint32_t)handler;
        uint64_t handler_low = h & 0xFFFF;
        uint64_t handler_med = (h >> 16) & 0xFFFF;

        uint64_t gate = handler_low | (cs << 16) | (ist << 32) | (flags << 40) |
                        (handler_med << 48);

        at[0] = gate;
}
#endif

enum { NONE = 0,
       USER_MODE = 1,
       STOP_IRQS = 2,
};

void register_idt_gate(int index, void (*handler)(void), int opts) {

        // TODO put these in a header
        uint16_t selector = 8; // kernel CS
        uint8_t rpl = (opts & USER_MODE) ? 3 : 0;
        uint8_t type =
            (opts & STOP_IRQS) ? 0xe : 0xf; // interrupt vs trap gates

        uint64_t flags = 0x80 | rpl << 5 | type;

        extern uint64_t idt;
        raw_set_idt_gate(&idt, index, handler, flags, selector, 0);
}

void install_isrs() {
        register_idt_gate(0, isr0, 0);
        register_idt_gate(1, isr1, 0);
        register_idt_gate(2, isr2, 0);
        register_idt_gate(3, isr3, 0);
        register_idt_gate(4, isr4, 0);
        register_idt_gate(5, isr5, 0);
        register_idt_gate(6, isr6, 0);
        register_idt_gate(7, isr7, 0);
        register_idt_gate(8, isr8, 0);
        register_idt_gate(9, isr9, 0);
        register_idt_gate(10, isr10, 0);
        register_idt_gate(11, isr11, 0);
        register_idt_gate(12, isr12, 0);
        register_idt_gate(13, isr13, 0);
        register_idt_gate(14, isr14, 0);
        register_idt_gate(15, isr15, 0);
        register_idt_gate(16, isr16, 0);
        register_idt_gate(17, isr17, 0);
        register_idt_gate(18, isr18, 0);
        register_idt_gate(19, isr19, 0);
        register_idt_gate(20, isr20, 0);
        register_idt_gate(21, isr21, 0);
        register_idt_gate(22, isr22, 0);
        register_idt_gate(23, isr23, 0);
        register_idt_gate(24, isr24, 0);
        register_idt_gate(25, isr25, 0);
        register_idt_gate(26, isr26, 0);
        register_idt_gate(27, isr27, 0);
        register_idt_gate(28, isr28, 0);
        register_idt_gate(29, isr29, 0);
        register_idt_gate(30, isr30, 0);
        register_idt_gate(31, isr31, 0);

        register_idt_gate(32, irq0, STOP_IRQS);
        register_idt_gate(33, irq1, STOP_IRQS);
        register_idt_gate(34, irq2, STOP_IRQS);
        register_idt_gate(35, irq3, STOP_IRQS);
        register_idt_gate(36, irq4, STOP_IRQS);
        register_idt_gate(37, irq5, STOP_IRQS);
        register_idt_gate(38, irq6, STOP_IRQS);
        register_idt_gate(39, irq7, STOP_IRQS);
        register_idt_gate(40, irq8, STOP_IRQS);
        register_idt_gate(41, irq9, STOP_IRQS);
        register_idt_gate(42, irq10, STOP_IRQS);
        register_idt_gate(43, irq11, STOP_IRQS);
        register_idt_gate(44, irq12, STOP_IRQS);
        register_idt_gate(45, irq13, STOP_IRQS);
        register_idt_gate(46, irq14, STOP_IRQS);
        register_idt_gate(47, irq15, STOP_IRQS);

        register_idt_gate(128, isr_syscall, USER_MODE);
        register_idt_gate(130, isr_panic, 0);
}

bool doing_exception_print = false;

#define NIRQS 16
void (*irq_handlers[NIRQS])(interrupt_frame *) = {
        [0] = timer_handler,
        [1] = keyboard_handler,
        [4] = x86_uart_irq_handler,
};

void panic_trap_handler(interrupt_frame *r);

void c_interrupt_shim(interrupt_frame *r) {
        // printf("Interrupt %i\n", r->interrupt_number);

        switch (r->interrupt_number) {
        case 14:
                page_fault(r);
                break;
        case 128:
                syscall_handler(r);
                break;
        case 130:
                panic_trap_handler(r);
                break;
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
                        panic("Interrupt %i recived I cannot deal with that "
                              "right now\n",
                              r->interrupt_number);
                }
                break;
        }
}

void syscall_handler(interrupt_frame *r) {
        struct syscall_ret ret;

        ret = do_syscall_with_table(frame_get(r, ARG0), frame_get(r, ARG1),
                                    frame_get(r, ARG2), frame_get(r, ARG3),
                                    frame_get(r, ARG4), frame_get(r, ARG5),
                                    frame_get(r, ARG6), r);

        uintptr_t flags = frame_get(r, FLAGS);
        if (ret.error) {
                frame_set(r, RET_VAL, ret.error);
                frame_set(r, FLAGS, flags | 1);
        } else {
                frame_set(r, RET_VAL, ret.value);
                frame_set(r, FLAGS, flags & ~1);
        }
}

void panic_trap_handler(interrupt_frame *r) {
        asm volatile("cli");
        printf("\n");
        printf("panic: trap at %#lx\n", frame_get(r, IP));
        print_registers(r);
        printf("backtrace from: %#lx\n", frame_get(r, BP));
        backtrace_from_with_ip(frame_get(r, BP), 20, frame_get(r, IP));
        panic();
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
        "Reserved",
};

void page_fault(interrupt_frame *r) {
        uintptr_t fault_addr;
        asm volatile("mov %%cr2, %0" : "=r"(fault_addr));

        const int PRESENT = 0x01;
        const int WRITE = 0x02;
        const int USERMODE = 0x04;
        const int RESERVED = 0x08;
        const int IFETCH = 0x10;

        // The vmm may choose to create pages that are not backed.  They will
        // appear as not having the PRESENT bit set.  In this case, ask it
        // if it can handle the situation and exit succesfully if it can
        extern int vmm_do_page_fault(uintptr_t fault_addr);
        if (vmm_do_page_fault(fault_addr)) {
                // debug: printf("page fault handled\n");
                return;
        }
        int code = r->error_code;

        if (r->error_code & RESERVED) {
                printf("Fault was caused by writing to a reserved field\n");
        }
        char *reason =
            code & PRESENT ? "protection violation" : "page not present";
        char *rw = code & WRITE ? "writing" : "reading";
        char *mode = code & USERMODE ? "user" : "kernel";
        char *type = code & IFETCH ? "instruction" : "data";

        if (code & USERMODE) {
                printf("** Segmentation fault **\n");
                printf("Thread: pid:%i,tid:%i performed an access violation\n",
                       running_process->pid, running_thread->tid);
                printf("Attempted to access: %s:%#lx, ", type, fault_addr);
                printf("Got: %s\n", reason);
                printf("Fault occured at %#lx\n", frame_get(r, IP));
                print_registers(r);
                backtrace_from(frame_get(r, BP), 10);

                if (running_process->pid == 1) {
                        panic("init died\n");
                } else {
                        kill_running_thread(-1);
                }
        }

        // set this after user mode dies, since this should only be true when
        // multiple exceptions are firing on top of each other.  By that point
        // the OS is probably going down for good.  I hope at least.
        if (doing_exception_print) {
                printf("--------- NEW FAULT ----------\n");
        }
        doing_exception_print = true;
        asm volatile("cli"); // no more irqs, we're dead

        const char *sentence = "Fault %s %s:%#lx because %s from %s mode.\n";
        printf(sentence, rw, type, fault_addr, reason, mode);

        if (fault_addr < 0x1000) {
                printf("NULL pointer access?\n");
        }
        uintptr_t ip = frame_get(r, IP);
        uintptr_t bp = frame_get(r, BP);
        printf("Fault occured at %#lx\n", ip);
        print_registers(r);
        printf("backtrace from: %#lx\n", bp);
        backtrace_from_with_ip(bp, 20, ip);
        uintptr_t real_sp = frame_get(r, SP);

#if I686
        // I686 interrupts to same privilege level do not save esp/ss
        if (r->ds & 0x03) {
                real_sp = r->user_esp;
        } else {
                asm volatile("mov %%esp, %0" : "=r"(real_sp));
                real_sp += sizeof(interrupt_frame);
                real_sp -= 8; // does not save esp, ss
        }
#endif

#if DO_STACK_DUMP
        printf("Stack dump: (sp at %#lx)\n", real_sp);
        dump_mem((char *)real_sp - 64, 128);
#endif
        panic();
}

void generic_exception(interrupt_frame *r) {
        if (doing_exception_print) {
                printf("--------- NEW FAULT ----------\n");
        }
        doing_exception_print = true;
        asm volatile("cli"); // no more irqs, we're dead

        printf("\n");
        printf("Unhandled exception at %#lx\n", frame_get(r, IP));
        printf("Fault: %s (%s), error code: %#04x\n",
               exception_codes[r->interrupt_number],
               exception_reasons[r->interrupt_number], r->error_code);
        print_registers(r);

        backtrace_from(frame_get(r, BP), 10);

#if DO_STACK_DUMP
        printf("Stack dump: (sp at %#lx)\n", frame_get(r, SP));
        dump_mem((char *)frame_get(r, SP) - 64, 128);
#endif

        panic();
}

/***
 * IRQ handlers
 ***/

volatile uint64_t timer_ticks = 0;

void timer_handler(interrupt_frame *r) {
        timer_ticks++;

        // Instead of having to scroll (at all) in real video memory, this
        // just gives me a framerate.
        // TODO:
        // Future direction: mark the buffer dirty and only update if needed
        vga_flush();

        // This must be done before the context swap, or it never gets done.
        send_eoi(r->interrupt_number - 32);

        if (ignore_timer_interrupt) {
                return;
        }

        switch_thread(SW_TIMEOUT);
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
        printf("Unhandled/unmasked IRQ %i received\n",
               r->interrupt_number - 32);
        send_eoi(r->interrupt_number - 32);
}

/* Utility functions */

void enable_irqs() {
        asm volatile("sti");
}

void disable_irqs() {
        asm volatile("cli");
}

