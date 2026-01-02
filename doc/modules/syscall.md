# syscall.ko - Custom Syscall Module

## Description

Demonstrates how a kernel module can register a new system call dynamically.
This module adds syscall #101 (`module_syscall`) that can be called from userspace.

## Purpose

- Shows how modules can extend the system call table
- Demonstrates persistent module state (call counter)
- Provides example of module-defined syscalls

## Functionality

On load, the module:
1. Registers syscall number 101 with the name "module_syscall"
2. Associates it with the `sys_module_syscall()` handler
3. Prints confirmation to kernel console

When the syscall is invoked:
1. Prints "This syscall was defined in a module"
2. Displays the address of the module's `calls` variable
3. Shows how many times the syscall has been called
4. Increments the call counter
5. Returns 0

## Usage

```bash
# Load the module
insmod syscall.ko

# Call from userspace (requires a program that makes the syscall)
# The syscall number is 101
```

## Example Userspace Program

```c
#include <nightingale.h>
#include <stdio.h>

int main() {
    syscall(101);  // Call the module-defined syscall
    return 0;
}
```

## Expected Output

On module load:
```
syscall registered
```

Each time syscall is invoked:
```
This syscall was defined in a module
This variable is at 0xffffffffXXXXXXXX
It has been called N times
```

## Source Code

**Location:** `/kernel/modules/syscall.c`

```c
#include <ng/mod.h>
#include <ng/syscall.h>
#include <stdio.h>

int calls = 0;

sysret sys_module_syscall() {
    printf("This syscall was defined in a module\n");
    printf("This variable is at %p\n", (void *)&calls);
    printf("It has been called %i times\n", calls++);
    return 0;
}

int init_mod(struct mod *) {
    int num = syscall_register(
        101, "module_syscall", sys_module_syscall, "module_syscall()", 0);
    printf("syscall registered\n");
    return MODINIT_SUCCESS;
}
```

## API Usage

**`syscall_register(num, name, handler, signature, flags)`**
- Registers a new system call at number `num`
- `name` is the syscall name (for debugging/tracing)
- `handler` is the function to call
- `signature` is a string describing arguments
- `flags` controls syscall behavior (0 for default)

## Notes

- The call counter persists across invocations, demonstrating module state
- Syscall number 101 is hardcoded - care must be taken to avoid conflicts
- The syscall remains registered even if the module is conceptually "unloaded"
- Module variables have valid kernel addresses that persist

## Security Considerations

- Modules run with full kernel privileges
- Syscall handlers must validate all userspace pointers
- No protection against malicious syscall implementations

## See Also

- `syscall(2)` - Generic syscall invocation
- `/interface/SYSCALLS` - System call manifest
- `/kernel/syscall.c` - Syscall infrastructure
