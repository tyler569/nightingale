
#include <basic.h>
#include <ng/x86/cpu.h>
#include <ng/debug.h>
#include <ng/print.h>
#include <ng/thread.h>
#include <ng/x86/32/cpu.h>

void print_registers(interrupt_frame *r) {
        uintptr_t cr3 = 0;
        asm volatile("mov %%cr3, %0" : "=a"(cr3));

        uintptr_t real_esp;
        bool using_estimated_esp = false;
        if (r->ds & 0x03) {
                real_esp = r->user_sp;
        } else {
                asm volatile("mov %%esp, %0" : "=r"(real_esp));
                real_esp += sizeof(interrupt_frame);
                real_esp -= 8; // does not save esp, ss
                using_estimated_esp = true;
        }

        printf("    eax: %8zx    ebx: %8zx\n", r->eax, r->ebx);
        printf("    ecx: %8zx    edx: %8zx\n", r->ecx, r->edx);
        printf("    esi: %8zx    edi: %8zx\n", r->esi, r->edi);
        printf("    esp: %8zx %c  ebp: %8zx\n", real_esp,
               using_estimated_esp ? '?' : ' ', r->bp);
        printf("    eip: %8zx    efl: [", r->ip);
        printf("%c%c%c%c%c%c] (%zx)\n",
                r->flags & 0x00000001 ? 'C' : ' ',
                r->flags & 0x00000004 ? 'P' : ' ',
                r->flags & 0x00000010 ? 'A' : ' ',
                r->flags & 0x00000040 ? 'Z' : ' ',
                r->flags & 0x00000080 ? 'S' : ' ',
                r->flags & 0x00000200 ? 'I' : ' ',
                r->flags);
        printf("    cr3: %8zx    pid: [%i:%i]\n", cr3,
                running_process->pid, running_thread->tid);
}

uintptr_t frame_get(interrupt_frame *r, int reg) {
        switch (reg) {
        case ARG0:
                return r->eax;
        case ARG1:
                return *(uintptr_t *)(r->user_sp);
        case ARG2:
                return *(uintptr_t *)(r->user_sp + 4);
        case ARG3:
                return *(uintptr_t *)(r->user_sp + 8);
        case ARG4:
                return *(uintptr_t *)(r->user_sp + 12);
        case ARG5:
                return *(uintptr_t *)(r->user_sp + 16);
        case ARG6:
                return *(uintptr_t *)(r->user_sp + 20);
        case RET_VAL:
                return r->eax;
        case RET_ERR:
                return r->ecx;
        case ARGC:
                return *(uintptr_t *)(r->user_sp + 4);
        case ARGV:
                return *(uintptr_t *)(r->user_sp + 8);
        case ENVP:
                return 0; // TODO r->edx;
        }
        return -1; // error
}

uintptr_t frame_set(interrupt_frame *r, int reg, uintptr_t value) {
        switch (reg) {
        case ARG0:
                r->eax = value;
                break;
        case ARG1:
                *(uintptr_t *)(r->user_sp) = value;
                break;
        case ARG2:
                *(uintptr_t *)(r->user_sp + 4) = value;
                break;
        case ARG3:
                *(uintptr_t *)(r->user_sp + 8) = value;
                break;
        case ARG4:
                *(uintptr_t *)(r->user_sp + 12) = value;
                break;
        case ARG5:
                *(uintptr_t *)(r->user_sp + 16) = value;
                break;
        case ARG6:
                *(uintptr_t *)(r->user_sp + 20) = value;
                break;
        case RET_VAL:
                r->eax = value;
                break;
        case RET_ERR:
                r->ecx = value;
                break;
        case ARGC:
                *(uintptr_t *)(r->user_sp + 4) = value;
                break;
        case ARGV:
                *(uintptr_t *)(r->user_sp + 8) = value;
                break;
        case ENVP:
                // TODO r->edx = value;
                break;
        }
        return value;
}
