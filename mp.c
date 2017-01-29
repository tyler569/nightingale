
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <kernel/cpu.h>
#include <kernel/mp.h>
#include "main.h"

/*
 * Preemptive multitasking, currently round-robin
 * planned ability to yield slot until defined time (i.e. don't schedule me for 100 ticks)
 *
 * Processes stored in ring
 *      _
 *  +->|_|--+ <- next_process
 *  |       v
 *  _       _
 * |_|<----|_|
 *
 *
 */

struct mp_process othr_process;
struct mp_process main_process = { (void **)0x1000000, 0, &othr_process };
struct mp_process othr_process = { (void **)0x1100000, 0, &main_process };
struct mp_process *current_process = &main_process;


void mp_initialize() {
    __asm__ ("mov %0, %%esp"
            :: "r" (current_process->stack_ptr));
    __asm__ ("mov %0, %%ebp"
            :: "r" (current_process->stack_ptr));
    __asm__ ("jmp kmain");
    abort();
    __builtin_unreachable();
}

// Create new mp_process and update pointers
void mp_newtask(struct regs *r, int **stack_ptr) {
    //TBI
}

// Called from timer IRQ handler 100x per second
void mp_taskswitch(struct regs *r, uint32_t timer_ticks) {
    void **temp;

    __asm__ ("movl %%esp, %0" : "=r" (temp) :: "eax");
    current_process->stack_ptr = temp;
    current_process = current_process->next_process;
    temp = current_process->stack_ptr;
    __asm__ ("movl %0, %%esp" :: "r" (temp) : "eax");

    if (temp == (void **)0x1100000) {
        struct regs *frame = (struct regs *)(0x1100000 - sizeof(struct regs));
                temp -= sizeof(struct regs);
        __asm__ ("movl %0, %%esp" :: "r" (temp) : "eax");
        __asm__ ("movl %0, %%ebp" :: "r" (temp) : "eax");
        
        memcpy(frame, r, sizeof(struct regs));
        frame->esp = 0x1100000;
        frame->ebp = 0x1100000;
        frame->eip = kmain2;

        __asm__ ("jmp irq_ret_start");
    }
}




    
