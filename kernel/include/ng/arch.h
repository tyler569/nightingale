#pragma once

#include <sys/cdefs.h>

BEGIN_DECLS

struct interrupt_frame;
struct thread;

void arch_init();
void arch_ap_setup(int cpu);
void arch_ap_init();
void arch_thread_context_save(struct thread *);
void arch_thread_context_restore(struct thread *);

END_DECLS
