#include <basic.h>
#include <stdio.h>
#include <nightingale.h>
#include <x86/cpu.h>

#if __kernel__
#include <ng/debug.h>
#include <ng/thread.h>
#endif

#define NEW_FORMAT 1

#if NEW_FORMAT
void print_registers(interrupt_frame *r)
{
    printf("ax %016lx bx %016lx cx %016lx dx %016lx\n", r->rax, r->rbx, r->rcx,
        r->rdx);
    printf("sp %016lx bp %016lx si %016lx di %016lx\n", r->user_sp, r->bp,
        r->rsi, r->rdi);
    printf(" 8 %016lx  9 %016lx 10 %016lx 11 %016lx\n", r->r8, r->r9, r->r10,
        r->r11);
    printf("12 %016lx 13 %016lx 14 %016lx 15 %016lx\n", r->r12, r->r13, r->r14,
        r->r15);
    printf("ip %016lx fl %016lx [", r->ip, r->flags);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c]\n", r->flags & 0x00000001 ? 'C' : ' ',
        r->flags & 0x00000004 ? 'P' : ' ', r->flags & 0x00000010 ? 'A' : ' ',
        r->flags & 0x00000040 ? 'Z' : ' ', r->flags & 0x00000080 ? 'S' : ' ',
        r->flags & 0x00000100 ? 'T' : ' ', r->flags & 0x00000200 ? 'I' : ' ',
        r->flags & 0x00000400 ? 'D' : ' ', r->flags & 0x00000800 ? 'O' : ' ',
        r->flags & 0x00010000 ? 'R' : ' ', r->flags & 0x00020000 ? 'V' : ' ',
        r->flags & 0x00040000 ? 'a' : ' ', r->flags & 0x00080000 ? 'v' : ' ',
        r->flags & 0x00100000 ? 'v' : ' ');
#ifdef __kernel__
    printf("cr3 %15lx pid %i:%i\n", cr3(), running_process->pid,
        running_thread->tid);
#endif
}
#else
void print_registers(interrupt_frame *r)
{
    printf("    rax: %16lx    r8 : %16lx\n", r->rax, r->r8);
    printf("    rbx: %16lx    r9 : %16lx\n", r->rbx, r->r9);
    printf("    rcx: %16lx    r10: %16lx\n", r->rcx, r->r10);
    printf("    rdx: %16lx    r11: %16lx\n", r->rdx, r->r11);
    printf("    rsp: %16lx    r12: %16lx\n", r->user_sp, r->r12);
    printf("    rbp: %16lx    r13: %16lx\n", r->bp, r->r13);
    printf("    rsi: %16lx    r14: %16lx\n", r->rsi, r->r14);
    printf("    rdi: %16lx    r15: %16lx\n", r->rdi, r->r15);
    printf("    rip: %16lx    rfl: [", r->ip);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c] (%lx)\n",
        r->flags & 0x00000001 ? 'C' : ' ', r->flags & 0x00000004 ? 'P' : ' ',
        r->flags & 0x00000010 ? 'A' : ' ', r->flags & 0x00000040 ? 'Z' : ' ',
        r->flags & 0x00000080 ? 'S' : ' ', r->flags & 0x00000100 ? 'T' : ' ',
        r->flags & 0x00000200 ? 'I' : ' ', r->flags & 0x00000400 ? 'D' : ' ',
        r->flags & 0x00000800 ? 'O' : ' ', r->flags & 0x00010000 ? 'R' : ' ',
        r->flags & 0x00020000 ? 'V' : ' ', r->flags & 0x00040000 ? 'a' : ' ',
        r->flags & 0x00080000 ? 'v' : ' ', r->flags & 0x00100000 ? 'v' : ' ',
        r->flags);
#if __kernel__
    uintptr_t cr3 = 0;
    asm volatile("mov %%cr3, %0" : "=a"(cr3));
    printf("    cr3: %16lx    pid: [%i:%i]\n", cr3, running_process->pid,
        running_thread->tid);
#endif
}
#endif
