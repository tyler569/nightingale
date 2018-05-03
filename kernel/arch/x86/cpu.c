
#include <basic.h>
#include <print.h>
#include <kthread.h>
#include <debug.h>
#include "cpu.h"

/*
 * cpu.c is for CPU-specific utilities, like
 * MSRs on Intel x86 CPUs
 */

u8 inb(port_addr_t port) {
    u8 result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outb(port_addr_t port, u8 data) {
    asm volatile ("outb %0, %1" :: "a"(data), "Nd"(port));
}

u16 inw(port_addr_t port) {
    u16 result;
    asm volatile ("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outw(port_addr_t port, u16 data) {
    asm volatile ("outw %0, %1" :: "a"(data), "Nd"(port));
}

// TODO: @Easy rename to inl
u32 ind(port_addr_t port) {
    u32 result;
    asm volatile ("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// TODO: @Easy rename to outl
void outd(port_addr_t port, u32 data) {
    asm volatile ("outl %0, %1" :: "a"(data), "Nd"(port));
}

u64 rdtsc() {
    u64 result;
    asm volatile ("rdtsc" : "=A"(result));
    return result;
}


void invlpg(uintptr_t address) {
    asm volatile ("invlpg (%0)" :: "b"(address) : "memory");
}


u64 rdmsr(u32 msr_id) {
    u64 result;
    asm volatile ("rdmsr" : "=A"(result) : "c"(msr_id));
    return result;
}

void wrmsr(u32 msr_id, u64 value) {
    asm volatile ("wrmsr" :: "c"(msr_id), "A"(value));
}

void print_registers(interrupt_frame *r) {
#define __human_readable_errors // TODO: control in Makefile for tests
#ifdef __human_readable_errors
    uintptr_t cr3 = 0;
    asm volatile ("mov %%cr3, %0" : "=a"(cr3));

    printf("    rax: %16lx    r8 : %16lx\n", r->rax, r->r8);
    printf("    rbx: %16lx    r9 : %16lx\n", r->rbx, r->r9);
    printf("    rcx: %16lx    r10: %16lx\n", r->rcx, r->r10);
    printf("    rdx: %16lx    r11: %16lx\n", r->rdx, r->r11);
    printf("    rsp: %16lx    r12: %16lx\n", r->user_rsp, r->r12);
    printf("    rbp: %16lx    r13: %16lx\n", r->rbp, r->r13);
    printf("    rsi: %16lx    r14: %16lx\n", r->rsi, r->r14);
    printf("    rdi: %16lx    r15: %16lx\n", r->rdi, r->r15);
    printf("    rip: %16lx    rfl: %16lx\n", r->rip, r->rflags);
    printf("    cr3: %16lx    pid: %16u\n", cr3, current_kthread->id);
    printf("    cr3: %16lx    pid: %16lx\n", cr3, current_kthread->id);
    dump_mem(current_kthread, sizeof(*current_kthread));

    // printf("    cr3: %l6lx\n", cr3); // <- TODO debug this shit!!
    // somehow that prints a different number entirely!

#else /* NOT __human_readable_errors */
    printf("dump:[v=1,rax=%#lx,rcx=%#lx,rbx=%#lx,rdx=%#lx,"
           "rsp=%#lx,rbp=%#lx,rsi=%#lx,rdi=%#lx,"
           "r8=%#lx,r9=%#lx,r10=%#lx,r11=%#lx,"
           "r12=%#lx,r13=%#lx,r14=%#lx,r15=%#lx,"
           "rip=%#lx,rflags=%#lx]\n",
           r->rax, r->rcx, r->rbx, r->rdx, r->user_rsp, r->rbp,
           r->rsi, r->rdi, r->r8, r->r9, r->r10, r->r11, r->r12,
           r->r13, r->r14, r->r15, r->rip, r->rflags);
#endif /* __human_readable_errors */
}

