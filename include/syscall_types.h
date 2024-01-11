#pragma once
#ifndef _SYSCALL_TYPES_H_
#define _SYSCALL_TYPES_H_

enum procstate {
    PS_COPYFDS = 0x0001,
    PS_SETRUN = 0x0002,
};

enum fault_type {
    NULL_DEREF,
    NULL_JUMP,
    ASSERT,
};

#endif // _SYSCALL_TYPES_H_
