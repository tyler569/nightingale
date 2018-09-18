
#include <basic.h>
#include <print.h>
#include <thread.h>
#include <debug.h>
#include "cpu.h"

/*
 * cpu.c is for CPU-specific utilities, like
 * MSRs on Intel x86 CPUs
 */

uint8_t inb(port_addr_t port) {
    uint8_t result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outb(port_addr_t port, uint8_t data) {
    asm volatile ("outb %0, %1" :: "a"(data), "Nd"(port));
}

uint16_t inw(port_addr_t port) {
    uint16_t result;
    asm volatile ("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outw(port_addr_t port, uint16_t data) {
    asm volatile ("outw %0, %1" :: "a"(data), "Nd"(port));
}

// TODO: @Easy rename to inl
uint32_t ind(port_addr_t port) {
    uint32_t result;
    asm volatile ("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// TODO: @Easy rename to outl
void outd(port_addr_t port, uint32_t data) {
    asm volatile ("outl %0, %1" :: "a"(data), "Nd"(port));
}

uint64_t rdtsc() {
    uint64_t result;
    asm volatile ("rdtsc" : "=A"(result));
    return result;
}


void set_vm_root(uintptr_t address) {
    asm volatile ("mov %0, %%cr3" :: "r"(address) : "memory");
}

void invlpg(uintptr_t address) {
    asm volatile ("invlpg (%0)" :: "b"(address) : "memory");
}

void flush_tlb(void) {
    asm volatile ("mov %%cr3, %%eax \n\t"
                  "mov %%eax, %%cr3" ::: "eax");
}

uint64_t rdmsr(uint32_t msr_id) {
    uint64_t result;
    asm volatile ("rdmsr" : "=A"(result) : "c"(msr_id));
    return result;
}

void wrmsr(uint32_t msr_id, uint64_t value) {
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
    printf("    rip: %16lx    rfl: [", r->rip);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c] (%lx)\n",
            r->rflags & 0x00000001 ? 'C' : ' ',
            r->rflags & 0x00000004 ? 'P' : ' ',
            r->rflags & 0x00000010 ? 'A' : ' ',
            r->rflags & 0x00000040 ? 'Z' : ' ',
            r->rflags & 0x00000080 ? 'S' : ' ',
            r->rflags & 0x00000100 ? 'T' : ' ',
            r->rflags & 0x00000200 ? 'I' : ' ',
            r->rflags & 0x00000400 ? 'D' : ' ',
            r->rflags & 0x00000800 ? 'O' : ' ',
            r->rflags & 0x00010000 ? 'R' : ' ',
            r->rflags & 0x00020000 ? 'V' : ' ',
            r->rflags & 0x00040000 ? 'a' : ' ',
            r->rflags & 0x00080000 ? 'v' : ' ',
            r->rflags & 0x00100000 ? 'v' : ' ',
            r->rflags);
    printf("    cr3: %16lx    pid: %16u\n", cr3, running_thread->pid);

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

