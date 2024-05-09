#pragma once

#include "stdint.h"
#include "sys/types.h"

#ifdef __x86_64__
#include <ng/arch_x86_64.h>
#else
#error "no supported architecture found"
#endif

struct interrupt_frame;
typedef struct interrupt_frame frame_t;

void halt_until_interrupt();
[[noreturn]] void halt_forever();
[[noreturn]] void halt_forever_interrupts_enabled();

void relax_busy_loop();

struct stream;
ssize_t write_debug(struct stream *, const void *str, size_t len);

void print_backtrace(frame_t *);
void print_frame(frame_t *);

uintptr_t get_frame_arg(frame_t *, int);
uintptr_t get_frame_syscall_arg(frame_t *, int);
void set_frame_arg(frame_t *, int, uintptr_t);
void set_frame_return(frame_t *, uintptr_t);

void debug_trap();

bool get_initrd_info(void **initrd_start, size_t *initrd_size);

uintptr_t get_vm_root();
void set_vm_root(uintptr_t root);

void jump_to_userland(uintptr_t entry, uintptr_t stack);
void new_frame(frame_t *, uintptr_t rip, uintptr_t rsp);
void new_user_frame(frame_t *, uintptr_t rip, uintptr_t rsp);
void copy_frame(frame_t *dst, frame_t *src);
void jump_to_frame(frame_t *);
