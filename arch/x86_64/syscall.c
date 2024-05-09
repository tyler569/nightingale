#include "sys/cdefs.h"
#include "x86_64.h"
#include <ng/arch-2.h>

void init_syscall() {
	uint64_t star = (USER_FAKE_SYSRET_CS << 48) | (KERNEL_CS << 32);
	uint64_t lstar = (uintptr_t)syscall_entry;
	uint64_t cstar = (uintptr_t)syscall_entry;
	uint64_t mask = FLAG_TF | FLAG_IF;
	uint64_t efer = read_msr(IA32_EFER);

	write_msr(IA32_STAR, star);
	write_msr(IA32_LSTAR, lstar);
	write_msr(IA32_CSTAR, cstar);
	write_msr(IA32_FMASK, mask);
	write_msr(IA32_EFER, efer | IA32_EFER_SCE);
}

USED void c_syscall_entry(uintptr_t a0, uintptr_t a1, uintptr_t a2,
	uintptr_t a3, uintptr_t a4, uintptr_t a5, int syscall_number, frame_t *f) {
	uintptr_t ret = do_syscall(a0, a1, a2, a3, a4, a5, syscall_number, f);
	set_frame_return(f, ret);
}
