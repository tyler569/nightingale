
#include <basic.h>
#include <term/print.h>
#include <panic.h>
#include "interrupt.h"
#include "irq.h"

void divide_by_zero_exception(interrupt_frame *r) {
    panic("Kernel divide by 0\n");
}

void page_fault(interrupt_frame *r) {

#define PRESENT     0x01
#define WRITE       0x02
#define USERMODE    0x04
#define RESERVED    0x08
#define IFETCH      0x10

    usize faulting_address;
    asm volatile ( "mov %%cr2, %0" : "=r"(faulting_address) );

    printf("Page fault accessing %p\n", faulting_address);

    if (r->error_code & PRESENT) {
        printf("Fault for protection\n");
    } else {
        printf("Fault for page not present\n");
    }
    if (r->error_code & WRITE) {
        printf("Fault on write\n");
    } else {
        printf("Fault on read\n");
    }
    if (r->error_code & USERMODE) {
        printf("Fault occurred in user space (CPL=3)\n");
    } else {
        printf("Fault occurred in kernel space\n");
    }
    if (r->error_code & RESERVED) {
        printf("Fault was caused by writing to a reserved field\n");
    }
    if (r->error_code & IFETCH) {
        printf("Fault was caused by an instruction fetch\n");
    } else {
        printf("Fault was not caused by an instruction fetch\n");
    }

    printf("Fault occured at %p\n", r->rip);
    panic();
}

void general_protection_exception(interrupt_frame *r) {
    panic("General Protection fault\nError code: 0x%x\n", r->error_code);
}

void panic_exception(interrupt_frame *r) {
    printf("Someone hit the panic interrupt at rip=%x!\n", r->rip);
    panic();
}

void syscall_handler(interrupt_frame *r) {
    printf("Syscall at 0x%x\n", r->rip);
    panic("Syscall not implemented\n");
}

void generic_exception(interrupt_frame *r) {
    printf("Unhandled exception at 0x%x\n", r->rip);
    panic("Exception: 0x%X Error code: 0x%x", r->interrupt_number, r->error_code);
}

