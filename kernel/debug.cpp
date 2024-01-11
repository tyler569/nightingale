#include <assert.h>
#include <elf.h>
#include <ng/debug.h>
#include <ng/mod.h>
#include <ng/panic.h>
#include <ng/string.h>
#include <ng/syscalls.h>
#include <ng/vmm.h>
#include <nightingale.h>
#include <stdio.h>

// TODO: factor
#define GET_BP(r) asm("mov %%rbp, %0" : "=r"(r));

static bool check_bp(uintptr_t bp)
{
    if (bp < 0x1000)
        return false;
    if (vmm_virt_to_phy(bp) == ~0u)
        return false;
    if (vmm_virt_to_phy(bp + 8) == ~0u)
        return false;
    return true;
}

static void print_frame(uintptr_t bp, uintptr_t ip, void *_)
{
    mod_sym sym = elf_find_symbol_by_address(ip);
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
        const elf_md *md = get_running_elf_metadata();
        if (!md) {
            printf("(%#018zx) <?+?>\n", ip);
            return;
        }
        const Elf_Sym *symbol = elf_symbol_by_address(md, ip);
        if (!symbol) {
            printf("(%#018zx) <?+?>\n", ip);
            return;
        }
        const char *name = elf_symbol_name(md, symbol);
        ptrdiff_t offset = ip - symbol->st_value;
        printf("(%#018zx) <%s+%#tx>\n", ip, name, offset);
    }
}

void backtrace(uintptr_t bp, uintptr_t ip,
    void (*callback)(uintptr_t, uintptr_t, void *), void *arg)
{
    if (ip)
        callback(bp, ip, arg);

    for (;;) {
        if (!check_bp(bp))
            return;
        auto *bp_ptr = (uintptr_t *)bp;
        bp = bp_ptr[0];
        ip = bp_ptr[1];

        callback(bp, ip, arg);
    }
}

void backtrace_from_with_ip(uintptr_t bp, uintptr_t ip)
{
    backtrace(bp, ip, print_frame, nullptr);
}

void backtrace_from_here()
{
    uintptr_t bp;
    GET_BP(bp);
    backtrace(bp, 0, print_frame, nullptr);
}

void backtrace_from_frame(interrupt_frame *frame)
{
    backtrace_from_with_ip(frame->bp, frame->ip);
}

void backtrace_all(void) { printf("backtrace_all: to reconstitute\n"); }

// hexdump memory

static char dump_byte_char(char c) { return isprint(c) ? c : '.'; }

static void print_byte_char_line(char *c)
{
    for (int i = 0; i < 16; i++) {
        printf("%c", dump_byte_char(c[i]));
    }
}

int hexdump(size_t len, char ptr[len])
{
    char *p = ptr;
    char *line = ptr;

    for (size_t i = 0; i < len; i++) {
        if (i % 16 == 0)
            printf("%08lx: ", (uintptr_t)(p + i));
        if (vmm_virt_to_phy((uintptr_t)(p + i)) == ~0u) {
            printf("EOM");
            return 0;
        }
        printf("%02hhx ", p[i]);
        if (i % 16 == 7)
            printf(" ");
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

void break_point()
{
    // This is called in assert() to give a place to put a
    // gdb break point
    int a = 10;
    volatile int *x = &a;
    *x = 20;
}

volatile int *volatile ptr;
void (*volatile fun)();

sysret sys_fault(enum fault_type type)
{
    switch (type) {
    case NULL_DEREF:
        *ptr = 1;
        break;
    case NULL_JUMP:
        fun();
        break;
    case ASSERT:
        assert(0);
    }

    return -EINVAL;
}
