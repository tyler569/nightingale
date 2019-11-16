
#pragma once
#ifndef _NIGHTINGALE_H_
#define _NIGHTINGALE_H_

#define HEAPDBG_SUMMARY 2
#define HEAPDBG_DETAIL 1

// debug the kernel heap
int heapdbg(int type);

// debug the local heap
void print_pool();
void summarize_pool();

// halt qemu
noreturn void haltvm();

long ng_time();


// TODO: copypasta from kernel/thread
enum procstate {
        PS_COPYFDS = 0x0001,
};

pid_t create(const char *executable);
int procstate(pid_t destination, enum procstate flags);

#endif // _NIGHTINGALE_H_

