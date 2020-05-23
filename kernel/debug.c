
#include <basic.h>
#include <ng/debug.h>
#include <ng/panic.h>
#include <ng/string.h>
#include <ng/syscall.h>
#include <ng/syscalls.h>
#include <ng/vmm.h>
#include <linker/elf.h>
#include <nightingale.h>
#include <assert.h>
#include <stdio.h>

#if X86_64
#define GET_BP(r) asm("mov %%rbp, %0" : "=r"(r));
#elif I686
#define GET_BP(r) asm("mov %%ebp, %0" : "=r"(r));
#endif


enum bt_opts {
        BACKTRACE_PRETTY       = (1 << 1),
};

const uintptr_t higher_half = 0x800000000000;

static bool user_mode(uintptr_t bp) {
        return bp < higher_half;
}

static bool check_bp(uintptr_t bp) {
        if (bp < 0x1000)  return false;
        if (vmm_virt_to_phy(bp) == -1)  return false;
        return true;
}

static void print_frame(uintptr_t bp, uintptr_t ip, enum bt_opts opts) {
        char sym_buf[64] = {0};
        if (opts & BACKTRACE_PRETTY && !user_mode(bp)) {
                elf_find_symbol_by_addr(&ngk_elfinfo, ip, sym_buf);
                printf("%s\n", sym_buf);
        } else {
                printf("    bp: %16zx    ip: %16zx\n", bp, ip);
        }
}

static void backtrace(uintptr_t bp, int max_frames, enum bt_opts opts) {
        uintptr_t ip;
        for (int i=0; i<max_frames; i++) {
                if (!check_bp(bp))  return;
                uintptr_t *bp_ptr = (uintptr_t *)bp;
                bp = bp_ptr[0];
                ip = bp_ptr[1];

                print_frame(bp, ip, opts);
        }
        printf("[.. frames omitted ..]\n");
}


void backtrace_from_with_ip(uintptr_t bp, int max_frames, uintptr_t ip) {
        print_frame(bp, ip, BACKTRACE_PRETTY);
        backtrace(bp, max_frames, BACKTRACE_PRETTY);
}

void backtrace_from_here(int max_frames) {
        uintptr_t bp;
        GET_BP(bp);
        backtrace(bp, max_frames, BACKTRACE_PRETTY);
}

// hexdump memory

static char dump_byte_char(char c) {
        return isprint(c) ? c : '.';
}

static void print_byte_char_line(char *c) {
        for (int i = 0; i < 16; i++) {
                printf("%c", dump_byte_char(c[i]));
        }
}

static int hexdump(size_t len, char ptr[len]) {
        char *p = ptr;
        char *line = ptr;

        for (int i=0; i<len; i++) {
                if (i % 16 == 0)  printf("%08lx: ", p + i);
                if (vmm_virt_to_phy((uintptr_t)(p + i)) == -1) {
                        printf("EOM");
                        return 0;
                }
                printf("%02hhx ", p[i]);
                if (i % 16 == 7)  printf(" ");
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
        case NULL_DEREF:
                return *x;
        case ASSERT:
                assert(0);
                break;
        default:
                return -EINVAL;
        }
}

#ifdef __GNUC__

__USED uintptr_t __stack_chk_guard = (~14882L);

__USED noreturn void __stack_chk_fail(void) {
        panic("Stack smashing detected");
        __builtin_unreachable();
}

#endif // __GNUC__

