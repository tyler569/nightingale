#pragma once

#include <ng/cpu.h>

#define cpu_local __attribute__((section("percpu")))
#define cpu_ref(var) (*(typeof(var) __seg_gs *)&var)

static inline void *cpu_ptr(void *local) {
	uintptr_t seg_gs = (uintptr_t)get_gs_base();
	return (void *)((uintptr_t)local + seg_gs);
}
