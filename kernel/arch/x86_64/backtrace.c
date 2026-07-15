#include <elf.h>
#include <ng/debug.h>
#include <ng/mod.h>
#include <ng/thread.h>
#include <ng/x86/interrupt.h>
#include <ng/x86/vmm.h>
#include <stdio.h>

static bool check_bp(uintptr_t bp) {
	if (bp < 0x1000)
		return false;
	if (vmm_virt_to_phy(bp) == ~0u)
		return false;
	if (vmm_virt_to_phy(bp + 8) == ~0u)
		return false;
	return true;
}

static void print_frame(uintptr_t bp, uintptr_t ip) {
	struct mod_sym sym = elf_find_symbol_by_address(ip);
	if (ip > HIGHER_HALF && sym.sym) {
		const elf_md *md = sym.mod ? sym.mod->md : &elf_ngk_md;
		const char *name = elf_symbol_name(md, sym.sym);
		ptrdiff_t offset = ip - sym.sym->st_value;
		if (sym.mod) {
			printf("(%#018zx) <%s:%s+%#tx> (%s @ %#018tx)\n", ip, sym.mod->name,
				name, offset, sym.mod->name, sym.mod->load_base);
		} else {
			printf("(%#018zx) <%s+%#tx>\n", ip, name, offset);
		}
	} else if (ip != 0) {
		const elf_md *md = running_process->elf_metadata;
		if (!md) {
			printf("(%#018zx) <?+?>\n", ip);
			return;
		}
		const Elf_Sym *sym = elf_symbol_by_address(md, ip);
		if (!sym) {
			printf("(%#018zx) <?+?>\n", ip);
			return;
		}
		const char *name = elf_symbol_name(md, sym);
		ptrdiff_t offset = ip - sym->st_value;
		printf("(%#018zx) <%s+%#tx>\n", ip, name, offset);
	}
}

void backtrace(uintptr_t bp, uintptr_t ip) {
	int frame_count = 0;

	while (bp) {
		print_frame(bp, ip);

		// Move to the next frame
		ip = *(uintptr_t *)(bp + 8);
		bp = *(uintptr_t *)bp;

		if (!check_bp(bp)) {
			break;
		}

		if (frame_count++ > 45) {
			printf("(too many frames)\n");
			break;
		}
	}
}

void backtrace_frame(struct interrupt_frame *frame) {
	backtrace(frame->bp, frame->ip);
}

void backtrace_context(jmp_buf ctx) {
	backtrace(ctx->__regs.bp, ctx->__regs.ip);
}
