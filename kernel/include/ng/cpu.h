
#pragma once
#ifndef NG_CPU_H
#define NG_CPU_H

#include <ng/basic.h>

// typedef struct interrupt_frame interrupt_frame;
// void print_registers(interrupt_frame*);

#if X86_64 || I686
#include "x86/cpu.h"
#else
#error "unsupported machine in cpu"
#endif

#endif // NG_CPU_H

