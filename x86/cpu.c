#include <basic.h>
#include <ng/debug.h>
#include <ng/thread.h>
#include <ng/x86/cpu.h>
#include <stdio.h>

/*
 * cpu.c is for CPU-specific utilities, like
 * MSRs on Intel x86 CPUs
 */

uint8_t inb(port_addr_t port) {
    uint8_t result;
    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outb(port_addr_t port, uint8_t data) {
    asm volatile("outb %0, %1" ::"a"(data), "Nd"(port));
}

uint16_t inw(port_addr_t port) {
    uint16_t result;
    asm volatile("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outw(port_addr_t port, uint16_t data) {
    asm volatile("outw %0, %1" ::"a"(data), "Nd"(port));
}

// TODO: @Easy rename to inl
uint32_t ind(port_addr_t port) {
    uint32_t result;
    asm volatile("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// TODO: @Easy rename to outl
void outd(port_addr_t port, uint32_t data) {
    asm volatile("outl %0, %1" ::"a"(data), "Nd"(port));
}

uint64_t rdtsc() {
    uint64_t result;
    asm volatile("rdtsc" : "=A"(result));
    return result;
}

void set_vm_root(uintptr_t address) {
    asm volatile("mov %0, %%cr3" ::"r"(address) : "memory");
}

void invlpg(uintptr_t address) {
    asm volatile("invlpg (%0)" ::"b"(address) : "memory");
}

void flush_tlb(void) {
    long temp = 0;
    asm volatile("mov %%cr3, %0 \n\t"
                 "mov %0, %%cr3 \n\t"
                 : "=r"(temp)
                 : "0"(temp));
}

uint64_t rdmsr(uint32_t msr_id) {
    uint64_t result;
    asm volatile("rdmsr" : "=A"(result) : "c"(msr_id));
    return result;
}

void wrmsr(uint32_t msr_id, uint64_t value) {
    asm volatile("wrmsr" ::"c"(msr_id), "A"(value));
}
