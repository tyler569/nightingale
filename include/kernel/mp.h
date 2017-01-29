
#pragma once

#include <stdint.h>
#include <kernel/cpu.h>

struct mp_process {
    void **stack_ptr;
    int32_t requested_wait;
    struct mp_process *next_process;
};

void mp_initialize();

void mp_newtask(struct regs *r, int **stack_ptr);

void mp_taskswitch(struct regs *r, uint32_t timer_ticks);

