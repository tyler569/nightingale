
#ifndef NIGHTINGALE_PROC_H
#define NIGHTINGALE_PROC_H

#include <basic.h>
#include <cpu/interrupt.h>

typedef u32 Pid;

typedef enum proc_state {
    PROC_RUNNING,
    PROC_WAITING,
    PROC_SLEEPING,
    PROC_KILLED,
    PROC_DEAD
} Proc_State;

typedef struct proc {
    Pid id;
    Proc_State state;

    interrupt_frame frame;
    struct proc *next;
    struct proc *parent;
} Proc;

typedef void Entrypoint();

void test_kernel_thread();

void do_process_swap(interrupt_frame *frame);
Pid  proc_create(Entrypoint entrypoint);
void proc_exit();

void proc_top();

#endif

