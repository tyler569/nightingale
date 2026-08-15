#pragma once

#include <stdint.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct interrupt_frame;
struct thread;

void arch_init();
void arch_ap_setup(int cpu);
void arch_ap_init();
void arch_thread_context_save(struct thread *);
void arch_thread_context_restore(struct thread *);
void arch_enable_irqs();
void arch_disable_irqs();

uint64_t arch_save_irqs();
void arch_restore_irqs(uint64_t);

[[noreturn]] void arch_halt_forever();

END_DECLS
