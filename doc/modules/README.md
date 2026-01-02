# Nightingale Kernel Modules

This directory contains documentation for Nightingale's example kernel modules.
These modules demonstrate various features of the kernel module system.

## Overview

Nightingale supports loadable kernel modules that can extend kernel functionality
at runtime. Modules are ELF object files (`.ko` extension) that are loaded using
the `insmod` command, which calls the `loadmod(2)` syscall.

## Module Loading

Modules are loaded using:
```bash
insmod module.ko
```

When a module is loaded:
1. The ELF file is parsed and loaded into kernel memory
2. Symbols are resolved and relocations applied
3. The module's `modinit()` function is called
4. If initialization succeeds, the module remains active

## Module Structure

Every module must have a `modinfo` structure:

```c
#include <ng/mod.h>

int modinit(struct mod *mod) {
    // Initialization code
    return MODINIT_SUCCESS; // or MODINIT_FAILURE
}

struct modinfo modinfo = {
    .name = "module_name",
    .init = modinit,
};
```

## Available Example Modules

### Basic Examples
- **testmod.ko** - Hello World module (prints message on load)
- **bss.ko** - BSS section and data alignment testing

### Feature Demonstration
- **procmod.ko** - Creates a `/proc` file
- **syscall.ko** - Registers a new system call
- **thread.ko** - Creates a kernel thread
- **file.ko** - Creates a character device

### Testing
- **crash.ko** - Intentionally crashes to test error handling

## Module Capabilities

Kernel modules can:
- Register new system calls
- Create `/proc` filesystem entries
- Create device files in `/dev`
- Start kernel threads
- Add device drivers
- Extend kernel functionality dynamically

## Limitations

Current limitations:
- Modules cannot be unloaded once loaded
- No dependency resolution between modules
- No module parameters support
- Limited symbol versioning

## Source Code

Module source code is located in:
```
/kernel/modules/
```

See individual module documentation below for detailed information about each module.
