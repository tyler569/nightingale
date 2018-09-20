
#pragma once
#ifndef NIGHTINGALE_ARCH_CPU_H
#define NIGHTINGALE_ARCH_CPU_H

#include <basic.h>

// typedef struct interrupt_frame interrupt_frame;
// void print_registers(interrupt_frame*);

#if defined(__x86_64__) || defined(__i686__)
#include "x86/cpu.h"
#else
#error "unsupported machine in cpu"
#endif

#endif

