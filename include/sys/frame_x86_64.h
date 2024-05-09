#pragma once

struct interrupt_frame {
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
	uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
	uint64_t int_no, err_code;
	uint64_t rip, cs, rflags, rsp, ss;
};

typedef struct interrupt_frame frame_t;
typedef struct interrupt_frame interrupt_frame;

#define FRAME_SYSCALL(frame) ((frame)->rax)
#define FRAME_ARG1(frame) ((frame)->rdi)
#define FRAME_ARG2(frame) ((frame)->rsi)
#define FRAME_ARG3(frame) ((frame)->rdx)
#define FRAME_ARG4(frame) ((frame)->rcx)
#define FRAME_ARG5(frame) ((frame)->r8)
#define FRAME_ARG6(frame) ((frame)->r9)
#define FRAME_RETURN(frame) ((frame)->rax)
#define FRAME_ARGC(frame) ((frame)->rdi)
#define FRAME_ARGV(frame) ((frame)->rsi)
#define FRAME_ENVP(frame) ((frame)->rdx)
