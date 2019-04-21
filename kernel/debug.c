
#include <ng/basic.h>
#include <ng/string.h>
#include <ng/print.h>
#include <ng/panic.h>
#include <ng/vmm.h>
#include <ng/debug.h>

#if X86_64
# define GET_BP(r) asm ("mov %%rbp, %0" : "=r"(r));
#elif I686
# define GET_BP(r) asm ("mov %%ebp, %0" : "=r"(r));
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
        return bt_test(x-1) + 1;
    } else {
        return backtrace_from_here(15);
    }
}

int backtrace_from(uintptr_t rbp_, int max_frames) {
    printf("backtrace from %zx:\n", rbp_);

    size_t *rbp = (size_t*)rbp_;
    size_t rip;
    int frame;

    for (frame=0; frame<max_frames; frame++) {
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

        /* TODO: #ifdef __human_readable_errors */
        printf("    bp: %16zx    ip: %16zx\n", rbp, rip);
        // unwind:
        if (rbp == 0 || rip == 0)  break;
        rbp = (size_t *)rbp[0];
    }
    if (frame == max_frames) {
        printf("[.. frames omitted ..]\n");
    }
    return 0;
}

char dump_byte_char(char c) {
    if (isalnum(c) || ispunct(c) || c == ' ') {
        return c;
    } else {
        return '.';
    }
}

void print_byte_char_line(char *c) {
    for (int i=0; i<16; i++) {
        printf("%c", dump_byte_char(c[i]));
    }
}

int dump_mem(void *ptr, size_t len) {
    char *p = ptr;

    for (int i=0; i<=len/16; i++) {
        // printf("%08lx: %#lx\n", p, vmm_virt_to_phy((uintptr_t)(p + i + 15)));
        if (vmm_virt_to_phy((uintptr_t)(p+15)) == -1) {
            if (vmm_virt_to_phy((uintptr_t)(p+7)) != -1) {
                printf("%08lx: %02hhx%02hhx %02hhx%02hhx %02hhx%02hhx %02hhx%02hhx\n", 
                    p, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]
                );
            }
            printf(" [ end of mapped memory ] \n");
            break;
        }
        printf("%08lx: %02hhx%02hhx %02hhx%02hhx %02hhx%02hhx %02hhx%02hhx %02hhx%02hhx %02hhx%02hhx %02hhx%02hhx %02hhx%02hhx   ", 
            p,
            p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
            p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
        print_byte_char_line(p);
        printf("\n");
        p += 16;
    }

    return 0;
}


long enabled_debug_print_modules = 0;
#define DBG_PRINT_MALLOC 0x01
#define DBG_PRINT_THREAD 0x02

void enable_debug_print(long module) {
    enabled_debug_print_modules |= module;
}

void disable_debug_print(long module) {
    enabled_debug_print_modules &= ~module;
}

int test_debug_print(long module) {
    return (module & enabled_debug_print_modules) > 0;
}


#ifdef __GNUC__

__used uintptr_t __stack_chk_guard = (~14882L);

__used noreturn void __stack_chk_fail(void) {
    panic("Stack smashing detected");
    __builtin_unreachable();
}

#endif // __GNUC__

