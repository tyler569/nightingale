#pragma once

#include <stdint.h>

typedef struct interrupt_frame {
	uint64_t x[32];
} interrupt_frame;

#define FRAME_SYSCALL(frame) ((frame)->x[8])
#define FRAME_ARG1(frame) ((frame)->x[0])
#define FRAME_ARG2(frame) ((frame)->x[1])
#define FRAME_ARG3(frame) ((frame)->x[2])
#define FRAME_ARG4(frame) ((frame)->x[3])
#define FRAME_ARG5(frame) ((frame)->x[4])
#define FRAME_ARG6(frame) ((frame)->x[5])
#define FRAME_RETURN(frame) ((frame)->x[0])
#define FRAME_ARGC(frame) ((frame)->x[0])
#define FRAME_ARGV(frame) ((frame)->x[1])
#define FRAME_ENVP(frame) ((frame)->x[2])
