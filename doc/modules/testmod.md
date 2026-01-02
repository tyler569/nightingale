# testmod.ko - Hello World Module

## Description

The simplest possible kernel module that demonstrates the basic module structure.
When loaded, it prints a "Hello World" message to the kernel console.

## Purpose

- Demonstrates minimal module implementation
- Verifies module loading works correctly
- Template for creating new modules

## Functionality

On load, the module:
1. Prints "Hello World from this kernel module!" to the kernel console
2. Returns `MODINIT_SUCCESS` to indicate successful initialization

## Usage

```bash
insmod testmod.ko
```

## Expected Output

Kernel console will show:
```
Hello World from this kernel module!
```

## Source Code

**Location:** `/kernel/modules/testmod.c`

```c
#include <ng/mod.h>
#include <stdio.h>

int modinit(struct mod *mod) {
    printf("Hello World from this kernel module!\n");
    return MODINIT_SUCCESS;
}

__USED struct modinfo modinfo = {
    .name = "test_mod",
    .init = modinit,
};
```

## Notes

- This is the recommended starting point for understanding kernel modules
- The module does nothing beyond printing the message
- Module name is "test_mod"

## See Also

- `insmod(1)` - Command to load modules
- `loadmod(2)` - System call for loading modules
- `/doc/modules/README.md` - Module system overview
