
#pragma once
#ifndef NG_SIGNAL_H
#define NG_SIGNAL_H

#include <basic.h>
#include <nc/signal.h>
#include <ng/syscall_consts.h>
#include <ng/cpu.h>

extern const unsigned char signal_handler_return[];

struct signal_context {
        struct interrupt_frame frame;
        int thread_state;
        void *sp;
        void *bp;
        char *stack;
        uintptr_t ip;
};

int send_signal(pid_t pid, int sig);
void handle_pending_signal();
void send_immidiate_signal_to_self(int sig);
void do_signal_call(int sig, sighandler_t handler);

#endif // NG_SIGNAL_H

