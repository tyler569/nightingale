
#pragma once
#ifndef NG_X86_32_CPU_H
#define NG_X86_32_CPU_H

#include <basic.h>

typedef struct interrupt_frame {
        uint32_t ds;
        uint32_t bp, edi, esi, edx, ebx, ecx, eax;
        uint32_t interrupt_number, error_code;
        uint32_t ip, cs, flags, user_sp, ss;
} interrupt_frame;

void print_registers(interrupt_frame *);

#endif // NG_X86_32_CPU_H

