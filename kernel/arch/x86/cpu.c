#include <ng/common.h>
#include <ng/debug.h>
#include <ng/thread.h>
#include <ng/x86/cpu.h>
#include <stdio.h>

// static uint32_t max_cpuid = 0; // FIXME cache
static uint32_t max_cpuid(void)
{
    uint32_t out[4];
    cpuid(0, 0, out);
    return out[_RAX];
}

#define checked_cpuid(L, SL, O) \
    do { \
        if ((L) >= max_cpuid()) { \
            return false; \
        } \
        cpuid(L, SL, O); \
    } while (0);

bool supports_feature(enum x86_feature feature)
{
    uint32_t out[4];

    switch (feature) {
    case _X86_FSGSBASE:
        checked_cpuid(7, 0, out);
        return out[_RBX] & 1;
    default:
        return false;
    }
}

extern inline uint64_t rdtsc(void);
extern inline uintptr_t cr3();
extern inline int cpunum();
extern inline void cpuid(uint32_t a, uint32_t c, uint32_t out[4]);
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
extern inline void set_tls_base(void *);
extern inline void set_gs_base(void *);
extern inline uintptr_t dr6(void);
