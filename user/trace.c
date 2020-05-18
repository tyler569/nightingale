
#include <stdio.h>
#include <stdlib.h>
#include <sys/trace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#include <ng/x86/cpu.h> // interrupt_frame

int exec(char **args) {
        int child = fork();
        if (child)  return child;

        strace(1);
        trace(0, TR_TRACEME, NULL, NULL);

        // return execve(args[0], args, NULL);
        printf("Hello World");
        exit(1);
}

void print_frame(interrupt_frame *r) {
        /*
        printf("syscall args:\n");
        printf("  ax: %p\n", r->rax);
        printf("  di: %p\n", r->rdi);
        printf("  si: %p\n", r->rsi);
        printf("  dx: %p\n", r->rdx);
        printf("  cx: %p\n", r->rcx);
        printf("  r8: %p\n", r->r8);
        printf("  r9: %p\n", r->r9);
        */
        /*
        printf("  ds: %#llx\n", r->ds);
        printf("  r15: %#llx\n", r->r15);
        printf("  r14: %#llx\n", r->r14);
        printf("  r13: %#llx\n", r->r13);
        printf("  r12: %#llx\n", r->r12);
        printf("  r11: %#llx\n", r->r11);
        printf("  r10: %#llx\n", r->r10);
        printf("  r9: %#llx\n", r->r9);
        printf("  r8: %#llx\n", r->r8);
        printf("  bp: %#llx\n", r->bp);
        printf("  rdi: %#llx\n", r->rdi);
        printf("  rsi: %#llx\n", r->rsi);
        printf("  rdx: %#llx\n", r->rdx);
        printf("  rbx: %#llx\n", r->rbx);
        printf("  rcx: %#llx\n", r->rcx);
        printf("  rax: %#llx\n", r->rax);
        printf("  interrupt_number: %#llx\n", r->interrupt_number);
        printf("  error_code: %#llx\n", r->error_code);
        printf("  ip: %#llx\n", r->ip);
        printf("  cs: %#llx\n", r->cs);
        printf("  flags: %#llx\n", r->flags);
        printf("  user_sp: %#llx\n", r->user_sp);
        printf("  ss: %#llx\n", r->ss);
        */
#if X86_64
        printf("  ax: %#llx\n", r->rax);
#else
        printf("  ax: %#llx\n", r->eax);
#endif
}

noreturn void fail(const char *str) {
        perror(str);
        exit(1);
}

int main(int argc, char **argv) {
        char **child_args = argv + 1;

        interrupt_frame r;

        int child = exec(NULL);

        int status;
        wait(&status);
        if (errno)  fail("wait");
        if (status != 0x40000)  fail("trace_wait");

        trace(child, TR_SYSCALL, NULL, NULL);

        while (true) {
                wait(&status);
                if (errno)  fail("wait");
                if (status < 256)  exit(0);

                printf("status: %#x\n", status);

                if ((status & ~0xFF) == TRACE_SYSCALL_ENTRY)
                        printf("syscall_enter: \n");
                if ((status & ~0xFF) == TRACE_SYSCALL_EXIT)
                        printf("syscall_exit: \n");

                trace(child, TR_GETREGS, NULL, &r);
                print_frame(&r);
                trace(child, TR_SYSCALL, NULL, NULL);
        }
}

