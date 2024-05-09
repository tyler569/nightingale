#include "ng/mem.h"
#include "x86_64.h"

#define INT_STACK_SIZE 8192

void init_int_stacks() {
	void *int_stack = kmem_alloc(INT_STACK_SIZE);
	void *nmi_stack = kmem_alloc(INT_STACK_SIZE);
	this_cpu->kernel_stack_top = (uintptr_t)int_stack + INT_STACK_SIZE;
	this_cpu->arch.tss.ist[0] = this_cpu->kernel_stack_top;
	this_cpu->arch.tss.ist[1] = (uintptr_t)nmi_stack + INT_STACK_SIZE;
}
