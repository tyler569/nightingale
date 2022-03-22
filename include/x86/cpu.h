#pragma once
#ifndef _X86_CPU_H_
#define _X86_CPU_H_

#include <basic.h>

#define IA32_APIC_BASE 27

#define NIRQS 16

typedef struct interrupt_frame {
    uint64_t ds;
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t bp, rdi, rsi, rdx, rbx, rcx, rax;
    uint64_t interrupt_number, error_code;
    uint64_t ip, cs, flags, user_sp, ss;
} interrupt_frame;

#define FRAME_SYSCALL(frame) ((frame)->rax)
#define FRAME_ARG1(frame) ((frame)->rdi)
#define FRAME_ARG2(frame) ((frame)->rsi)
#define FRAME_ARG3(frame) ((frame)->rdx)
#define FRAME_ARG4(frame) ((frame)->rcx)
#define FRAME_ARG5(frame) ((frame)->r8)
#define FRAME_ARG6(frame) ((frame)->r9)
#define FRAME_RETURN(frame) ((frame)->rax)
#define FRAME_ARGC(frame) ((frame)->rdi)
#define FRAME_ARGV(frame) ((frame)->rsi)
#define FRAME_ENVP(frame) ((frame)->rdx)

typedef uint16_t port_addr_t;

#define INTERRUPT_ENABLE 0x200
#define TRAP_FLAG 0x100

_Noreturn void halt();

inline uint64_t rdtsc() { return __builtin_ia32_rdtsc(); }

inline uintptr_t cr3()
{
    uintptr_t cr3 = 0;
    asm volatile("mov %%cr3, %0" : "=a"(cr3));
    return cr3;
}

inline int cpunum()
{
    int cpunum;
    asm("mov $1, %%eax;"
        "cpuid;"
        "shrl $24, %%ebx;"
        : "=b"(cpunum)
        :
        : "eax", "ecx", "edx");
    return cpunum;
}

inline void cpuid(uintptr_t a, uintptr_t c, uintptr_t out[4])
{
    asm("cpuid"
        : "=a"(out[0]), "=b"(out[1]), "=c"(out[2]), "=d"(out[3])
        : "0"(a), "2"(c));
}

inline void enable_bits_cr4(uintptr_t bitmap)
{
    uintptr_t tmp;
    asm volatile("mov %%cr4, %%rax\n\t"
                 "or %0, %%rax\n\t"
                 "mov %%rax, %%cr4\n\t"
                 :
                 : "r"(bitmap)
                 : "rax");
}

inline uint8_t inb(port_addr_t port)
{
    uint8_t result;
    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

inline void outb(port_addr_t port, uint8_t data)
{
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

inline uint16_t inw(port_addr_t port)
{
    uint16_t result;
    asm volatile("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

inline void outw(port_addr_t port, uint16_t data)
{
    asm volatile("outw %0, %1" : : "a"(data), "Nd"(port));
}

inline uint32_t ind(port_addr_t port)
{
    uint32_t result;
    asm volatile("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

inline void outd(port_addr_t port, uint32_t data)
{
    asm volatile("outl %0, %1" : : "a"(data), "Nd"(port));
}

inline void set_vm_root(uintptr_t address)
{
    asm volatile("mov %0, %%cr3" : : "r"(address) : "memory");
}

inline void invlpg(uintptr_t address)
{
    asm volatile("invlpg (%0)" : : "b"(address) : "memory");
}

inline void flush_tlb(void)
{
    long temp = 0;
    asm volatile("mov %%cr3, %0 \n\t"
                 "mov %0, %%cr3 \n\t"
                 : "=r"(temp)
                 : "0"(temp));
}

inline uint64_t rdmsr(uint32_t msr_id)
{
    uint32_t a, d;
    asm volatile("rdmsr" : "=a"(a), "=d"(d) : "c"(msr_id));
    return ((uint64_t)d << 32) + a;
}

inline void wrmsr(uint32_t msr_id, uint64_t value)
{
    uint32_t a = value, d = value >> 32;
    asm volatile("wrmsr" : : "c"(msr_id), "a"(a), "d"(d));
}

inline void set_tls_base(void *tlsbase)
{
    extern int have_fsgsbase;
    if (have_fsgsbase)
        asm volatile("wrfsbase %0" ::"r"(tlsbase));
    else
        wrmsr(0xC0000100, (uintptr_t)tlsbase);
}

#endif // _X86_CPU_H_
