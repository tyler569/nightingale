#include <basic.h>
#include <assert.h>
#include <elf.h>
#include <ng/debug.h>
#include <ng/mod.h>
#include <ng/panic.h>
#include <ng/serial.h>
#include <ng/string.h>
#include <ng/syscall.h>
#include <ng/syscalls.h>
#include <ng/vmm.h>
#include <nightingale.h>
#include <stdio.h>


void s2printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char buf[256] = {0};
    size_t len = vsnprintf(buf, 256, format, args);
    serial2_write_str(buf, len);
}

// TODO: factor
#define GET_BP(r) asm("mov %%rbp, %0" : "=r"(r));

#if X86_64
const uintptr_t higher_half = 0x800000000000;
#else
const uintptr_t higher_half = 0x80000000;
#endif

static bool user_mode(uintptr_t bp) {
    return bp < higher_half;
}

static bool check_bp(uintptr_t bp) {
    if (bp < 0x1000) return false;
    if (vmm_virt_to_phy(bp) == -1) return false;
    if (vmm_virt_to_phy(bp + 8) == -1) return false;
    return true;
}

static void print_frame(uintptr_t bp, uintptr_t ip) {
    struct mod_sym sym = elf_find_symbol_by_address(ip);
    if (ip > higher_half && sym.sym) {
        const char *name = elf_symbol_name(sym.mod, sym.sym);
        ptrdiff_t offset = ip - sym.sym->st_value;
        printf("(%#016zx) <%s+%#x>\n", ip, name, offset);
    } else if (ip != 0) {
        printf("    bp: %16zx    ip: %16zx\n", bp, ip);
    }
}

void backtrace(uintptr_t bp, void (*callback)(uintptr_t, uintptr_t)) {
    uintptr_t ip;
    for (int i = 0;; i++) {
        if (!check_bp(bp)) return;
        uintptr_t *bp_ptr = (uintptr_t *)bp;
        bp = bp_ptr[0];
        ip = bp_ptr[1];

        callback(bp, ip);
    }
}


void backtrace_from_with_ip(uintptr_t bp, uintptr_t ip) {
    print_frame(bp, ip);
    backtrace(bp, print_frame);
}

void backtrace_from_here() {
    uintptr_t bp;
    GET_BP(bp);
    backtrace(bp, print_frame);
}

void backtrace_all(void) {
    list_for_each(struct thread, th, &all_threads, all_threads) {
        if (th == running_thread) continue;
        printf("--- [%i:%i] (%s):\n", th->tid, th->proc->pid, th->proc->comm);
        backtrace_from_with_ip(th->kernel_ctx->__regs.bp,
                               th->kernel_ctx->__regs.ip);
        printf("\n");
    }
}

static void print_perf_frame(uintptr_t bp, uintptr_t ip) {
    struct mod_sym sym = elf_find_symbol_by_address(ip);
    if (!sym.sym) return;
    const char *name = elf_symbol_name(sym.mod, sym.sym);
    s2printf("%s\n", sym.sym);
}

void print_perf_trace(uintptr_t bp, uintptr_t ip) {
    if (bp < 0xFFFF000000000000) return;
    print_perf_frame(bp, ip);
    backtrace(bp, print_perf_frame);
    s2printf("1\n\n");
}

// hexdump memory

static char dump_byte_char(char c) {
    return isprint(c) ? c : '.';
}

static void print_byte_char_line(char *c) {
    for (int i = 0; i < 16; i++) { printf("%c", dump_byte_char(c[i])); }
}

int hexdump(size_t len, char ptr[len]) {
    char *p = ptr;
    char *line = ptr;

    for (int i = 0; i < len; i++) {
        if (i % 16 == 0) printf("%08lx: ", p + i);
        if (vmm_virt_to_phy((uintptr_t)(p + i)) == -1) {
            printf("EOM");
            return 0;
        }
        printf("%02hhx ", p[i]);
        if (i % 16 == 7) printf(" ");
        if (i % 16 == 15) {
            printf("   ");
            print_byte_char_line(line);
            line = p + i + 1;
            printf("\n");
        }
    }
    return 0;
}

// random things

void break_point() {
    // This is called in assert() to give a place to put a
    // gdb break point
    int a = 10;
    volatile int *x = &a;
    *x = 20;
}

sysret sys_haltvm(int exit_code) {
#if X86
    outb(0x501, exit_code);
#else
#endif
    panic("sys_haltvm called on an unsupported platform");
}

sysret sys_fault(enum fault_type type) {
    volatile int *x = 0;
    switch (type) {
    case NULL_DEREF: return *x;
    case ASSERT: assert(0); break;
    default: return -EINVAL;
    }
}

#ifdef __GNUC__

__USED uintptr_t __stack_chk_guard = (~14882L);

__USED noreturn void __stack_chk_fail(void) {
    panic("Stack smashing detected");
    __builtin_unreachable();
}

#endif // __GNUC__
