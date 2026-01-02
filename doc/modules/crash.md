# crash.ko - Crash Testing Module

## Description

A deliberately faulty kernel module designed to crash during initialization.
Used for testing kernel crash handling, debugging infrastructure, and
module loading error recovery.

## Purpose

- Tests kernel panic/crash handling
- Validates backtrace generation
- Verifies kernel stability after module crashes
- Debugging tool for kernel fault handlers

## Functionality

On load, the module:
1. Prints "This module will now crash"
2. Attempts to dereference NULL pointer (address 0)
3. **NEVER returns** - triggers page fault

This will cause:
- Page fault exception (invalid memory access)
- Kernel panic or crash handler activation
- Backtrace generation
- System may halt or recover depending on fault handler

## Usage

```bash
# WARNING: This WILL crash the kernel!
insmod crash.ko
```

## Expected Output

```
This module will now crash
[CRASH - Page fault or kernel panic]
[Backtrace showing crash location]
```

The system may:
- Print a backtrace with register dump
- Halt completely
- Drop to debugger if connected
- Attempt recovery (unlikely)

## Source Code

**Location:** `/kernel/modules/crash.c`

```c
#include <ng/mod.h>
#include <stdio.h>

int modinit(struct mod *mod) {
    printf("This module will now crash\n");
    int out;
    asm volatile("movl (0), %0" : "=r"(out));
    return MODINIT_SUCCESS;  // Never reached
}

__USED struct modinfo modinfo = {
    .name = "test_mod",
    .init = modinit,
};
```

## What It Does

The inline assembly:
```c
asm volatile("movl (0), %0" : "=r"(out));
```

Translates to:
- `movl (0), %eax` - Move 32-bit value from address 0 to register
- Address 0 is NULL and not mapped
- CPU generates page fault exception
- Kernel fault handler is invoked

## Use Cases

This module is useful for:
- Testing kernel panic infrastructure
- Validating backtrace accuracy
- Verifying crash dumps
- Testing recovery mechanisms
- Debugging kernel fault handlers
- Training/demonstrating kernel crashes

## Debugging

When this module crashes, use:
```bash
# After crash, if serial output available
./bt.sh
```

This will symbolicate the backtrace to show:
- Exact line where crash occurred
- Call stack leading to crash
- Register state at crash time

## Safety Warnings

⚠️ **WARNING**: This module WILL crash the kernel!

- All unsaved data may be lost
- System will likely halt
- File system may be corrupted if not sync'd
- Use only in virtual machines or test environments
- **NOT for production systems**

## Notes

- Module name is "test_mod" (same as testmod.ko)
- Never reaches `MODINIT_SUCCESS` return
- The `volatile` keyword prevents compiler from optimizing out the crash
- Direct memory access to NULL is guaranteed to fault on x86_64

## See Also

- `./bt.sh` - Backtrace symbolication script
- `/kernel/panic.c` - Panic handling code
- `/arch/x86_64/interrupt.c` - Exception handlers
- `fault(2)` - Syscall for triggering controlled faults
