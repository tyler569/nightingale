#include <basic.h>
#include <ng/debug.h>
#include <ng/thread.h>
#include <stdio.h>
#include <x86/cpu.h>

/*
 * cpu.c is for CPU-specific utilities, like
 * MSRs on Intel x86 CPUs
 */

extern inline uint64_t rdtsc(void);
extern inline uintptr_t cr3();
extern inline int cpunum();
extern inline void cpuid(uintptr_t a, uintptr_t c, uintptr_t out[4]);
extern inline void enable_bits_cr4(uintptr_t bitmap);
extern inline void set_tls_base(void *tlsbase);
extern inline uint8_t inb(port_addr_t port);
extern inline void outb(port_addr_t port, uint8_t data);
extern inline uint16_t inw(port_addr_t port);
extern inline void outw(port_addr_t port, uint16_t data);
extern inline uint32_t ind(port_addr_t port);
extern inline void outd(port_addr_t port, uint32_t data);
extern inline void set_vm_root(uintptr_t address);
extern inline void invlpg(uintptr_t address);
extern inline void flush_tlb(void);
extern inline uint64_t rdmsr(uint32_t msr_id);
extern inline void wrmsr(uint32_t msr_id, uint64_t value);
