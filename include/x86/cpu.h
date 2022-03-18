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

uint8_t inb(port_addr_t port);
void outb(port_addr_t port, uint8_t data);
uint16_t inw(port_addr_t port);
void outw(port_addr_t port, uint16_t data);
uint32_t ind(port_addr_t port);
void outd(port_addr_t port, uint32_t data);

inline uint64_t rdtsc(void);
void set_vm_root(uintptr_t);
void invlpg(uintptr_t);
void flush_tlb(void);
uint64_t rdmsr(uint32_t msr_id);
void wrmsr(uint32_t msr_id, uint64_t value);

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

#endif // _X86_CPU_H_
