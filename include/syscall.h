#pragma once
#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <basic.h>
#include <stddef.h>
#include <stdint.h>

#define __syscall0(num) \
    ({ \
        register intptr_t _snum = (intptr_t)(num); \
        intptr_t _sret; \
        asm("int $0x80" : "=a"(_sret) : "0"(_snum) : "memory"); \
        _sret; \
    })

#define __syscall1(num, a1) \
    ({ \
        register intptr_t _snum = (intptr_t)(num); \
        register intptr_t _sa1 = (intptr_t)(a1); \
        intptr_t _sret; \
        asm("int $0x80" : "=a"(_sret) : "0"(_snum), "D"(_sa1) : "memory"); \
        _sret; \
    })

#define __syscall2(num, a1, a2) \
    ({ \
        register intptr_t _snum = (intptr_t)(num); \
        register intptr_t _sa1 = (intptr_t)(a1); \
        register intptr_t _sa2 = (intptr_t)(a2); \
        intptr_t _sret; \
        asm("int $0x80" \
            : "=a"(_sret) \
            : "0"(_snum), "D"(_sa1), "S"(_sa2) \
            : "memory"); \
        _sret; \
    })

#define __syscall3(num, a1, a2, a3) \
    ({ \
        register intptr_t _snum = (intptr_t)(num); \
        register intptr_t _sa1 = (intptr_t)(a1); \
        register intptr_t _sa2 = (intptr_t)(a2); \
        register intptr_t _sa3 = (intptr_t)(a3); \
        intptr_t _sret; \
        asm("int $0x80" \
            : "=a"(_sret) \
            : "0"(_snum), "D"(_sa1), "S"(_sa2), "d"(_sa3) \
            : "memory"); \
        _sret; \
    })

#define __syscall4(num, a1, a2, a3, a4) \
    ({ \
        register intptr_t _snum = (intptr_t)(num); \
        register intptr_t _sa1 = (intptr_t)(a1); \
        register intptr_t _sa2 = (intptr_t)(a2); \
        register intptr_t _sa3 = (intptr_t)(a3); \
        register intptr_t _sa4 = (intptr_t)(a4); \
        intptr_t _sret; \
        asm("int $0x80" \
            : "=a"(_sret) \
            : "0"(_snum), "D"(_sa1), "S"(_sa2), "d"(_sa3), "c"(_sa4) \
            : "memory"); \
        _sret; \
    })

/*
 * If you must use a specific register, but your Machine Constraints
 * do not provide sufficient control to select the specific register
 * you want, local register variables may provide a solution.
 *
 * [...] using the variable as an input or output operand to the asm
 * guarantees that the specified register is used for that operand.
 */

#define __syscall5(num, a1, a2, a3, a4, a5) \
    ({ \
        register intptr_t _snum = (intptr_t)(num); \
        register intptr_t _sa1 = (intptr_t)(a1); \
        register intptr_t _sa2 = (intptr_t)(a2); \
        register intptr_t _sa3 = (intptr_t)(a3); \
        register intptr_t _sa4 = (intptr_t)(a4); \
        register intptr_t _sa5 asm("r8") = (intptr_t)(a5); \
        intptr_t _sret; \
        asm("int $0x80" \
            : "=a"(_sret) \
            : "0"(_snum), "D"(_sa1), "S"(_sa2), "d"(_sa3), "c"(_sa4), \
            "r"(_sa5) \
            : "memory"); \
        _sret; \
    })

#define __syscall6(num, a1, a2, a3, a4, a5, a6) \
    ({ \
        register intptr_t _snum = (intptr_t)(num); \
        register intptr_t _sa1 = (intptr_t)(a1); \
        register intptr_t _sa2 = (intptr_t)(a2); \
        register intptr_t _sa3 = (intptr_t)(a3); \
        register intptr_t _sa4 = (intptr_t)(a4); \
        register intptr_t _sa5 asm("r8") = (intptr_t)(a5); \
        register intptr_t _sa6 asm("r9") = (intptr_t)(a6); \
        intptr_t _sret; \
        asm("int $0x80" \
            : "=a"(_sret) \
            : "0"(_snum), "D"(_sa1), "S"(_sa2), "d"(_sa3), "c"(_sa4), \
            "r"(_sa5), "r"(_sa6) \
            : "memory"); \
        _sret; \
    })

#if 0
intptr_t __syscall0(int syscall_num);

intptr_t __syscall1(int syscall_num, intptr_t arg1);

intptr_t __syscall2(int syscall_num, intptr_t arg1, intptr_t arg2);

intptr_t __syscall3(
    int syscall_num, intptr_t arg1, intptr_t arg2, intptr_t arg3);

intptr_t __syscall4(int syscall_num, intptr_t arg1, intptr_t arg2,
    intptr_t arg3, intptr_t arg4);

intptr_t __syscall5(int syscall_num, intptr_t arg1, intptr_t arg2,
    intptr_t arg3, intptr_t arg4, intptr_t arg5);

intptr_t __syscall6(int syscall_num, intptr_t arg1, intptr_t arg2,
    intptr_t arg3, intptr_t arg4, intptr_t arg5, intptr_t arg6);
#endif

#endif // _SYSCALL_H_
