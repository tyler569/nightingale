
#include <basic.h>
#include <arch/x86/cpu.h>

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


u64 rdmsr(u32 msr_id) {
    u64 result;
    asm volatile ("rdmsr" : "=A"(result) : "c"(msr_id));
    return result;
}

void wrmsr(u32 msr_id, u64 value) {
    asm volatile ("wrmsr" :: "c"(msr_id), "A"(value));
}

