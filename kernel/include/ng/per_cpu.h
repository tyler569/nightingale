#pragma once

#include <ng/cpu.h>
#include <stdint.h>

// cpu_local variables are linked into the read-only percpu template, which is
// never accessed directly. Each cpu gets a copy of the template, and its gs
// base holds the distance from the template to that copy, so ordinary
// accesses compile to %gs:var(%rip).
#define cpu_local __attribute__((section("percpu"))) __seg_gs

// This cpu's gs base, readable without rdgsbase / rdmsr.
extern cpu_local uintptr_t this_cpu_off;

// Flat pointer to this cpu's copy of a cpu_local variable, for code that
// needs a generic pointer (memset, descriptor table bases, ...).
#define cpu_ptr(local) \
	((typeof_unqual(*(local)) *)((uintptr_t)(local) + this_cpu_off))
