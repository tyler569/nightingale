
#include <basic.h>
#include <ng/debug.h>
#include <ng/elf.h>
#include <ng/panic.h>
#include <ng/print.h>
#include <ng/string.h>
#include <ng/syscall.h>
#include <ng/syscalls.h>
#include <ng/vmm.h>
#include <nc/nightingale.h>
#include <nc/assert.h>

#if X86_64
#define GET_BP(r) asm("mov %%rbp, %0" : "=r"(r));
#elif I686
#define GET_BP(r) asm("mov %%ebp, %0" : "=r"(r));
#endif

int backtrace_from_here(int max_frames) {
        printf("backtrace:\n");

        uintptr_t *rbp;
        // rbp = (uintptr_t *)(&rbp - 3);
        GET_BP(rbp);

        backtrace_from((uintptr_t)rbp, max_frames);
        return 0;
}

int bt_test(int x) {
        if (x > 1) {
                return bt_test(x - 1) + 1;
        } else {
                return backtrace_from_here(15);
        }
}

bool do_fancy_exception = true;

int backtrace_from(uintptr_t rbp_, int max_frames) {
        size_t *rbp = (size_t *)rbp_;
        size_t rip;
        int frame;
        bool is_kernel_mode;

        // UGH!
#if X86_64
        if (rbp_ >= 0xFFFF000000000000) {
#elif I686
        if (rbp_ >= 0x80000000) {
#endif
                is_kernel_mode = true;
        } else {
                is_kernel_mode = false;
        }

        // printf("frames start: %i %i", (int)is_kernel_mode, max_frames);

        for (frame = 0; frame < max_frames; frame++) {
                if (vmm_virt_to_phy((uintptr_t)(rbp + 1)) == -1) {
                        // don't spill to unmapped memory and crash again
                        printf("end of memory\n");
                        break;
                }
                if (rbp == 0) {
                        printf("top of stack\n");
                        break; // end of trace
                } else {
                        rip = rbp[1];
                }

                if (do_fancy_exception && is_kernel_mode) {
                        char buf[256] = {0};
                        elf_find_symbol_by_addr(&ngk_elfinfo, rip, buf);
                        printf("%s\n", buf);
                } else {
                        printf("    bp: %16zx    ip: %16zx\n", rbp, rip);
                }
                // unwind:
                if (rbp == 0 || rip == 0)
                        break;
                rbp = (size_t *)rbp[0];
        }
        if (frame == max_frames) {
                printf("[.. frames omitted ..]\n");
        }
        return 0;
}

void backtrace_from_with_ip(uintptr_t rbp, int max_frames, uintptr_t ip) {
        int is_kernel_mode;
        // double UGH - copypasta from above
#if X86_64
        if (rbp >= 0xFFFF000000000000) {
#elif I686
        if (rbp >= 0x80000000) {
#endif
                is_kernel_mode = true;
        } else {
                is_kernel_mode = false;
        }

        if (do_fancy_exception && is_kernel_mode) {
                char buf[256] = {0};
                elf_find_symbol_by_addr(&ngk_elfinfo, ip, buf);
                printf("%s\n", buf);
        } else {
                printf("    bp: %16zx    ip: %16zx\n", rbp, ip);
        }

        backtrace_from(rbp, max_frames);
}

char dump_byte_char(char c) {
        return isprint(c) ? c : '.';
}

void print_byte_char_line(char *c) {
        for (int i = 0; i < 16; i++) {
                printf("%c", dump_byte_char(c[i]));
        }
}

int dump_mem(void *ptr, size_t len) {
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

noinline void break_point() {
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
        __builtin_unreachable();
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

_used uintptr_t __stack_chk_guard = (~14882L);

_used noreturn void __stack_chk_fail(void) {
        panic("Stack smashing detected");
        __builtin_unreachable();
}

#endif // __GNUC__

