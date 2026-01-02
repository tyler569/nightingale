# Nightingale Kernel-Internal API Documentation

This directory contains manual pages for kernel-internal functions and APIs (man section 9).
These are the functions available to kernel code and kernel modules.

## Viewing Manual Pages

You can view these pages with any man page viewer or text editor.
If you have `groff` installed:

```bash
groff -man -Tascii kthread_create.9 | less
```

Or with `man` if your MANPATH includes this directory:

```bash
man 9 kthread_create
```

## APIs by Category

### Thread Management
- **kthread_create**(9) - create a kernel thread
- **sleep_thread**(9) - sleep the current thread

### Proc Filesystem
- **make_proc_file**(9) - create a /proc file
- **proc_sprintf**(9) - write formatted data to proc file
- **new_proc_vnode**(9) - create proc vnode

### System Call Registration
- **syscall_register**(9) - register a new system call dynamically

### Memory Management
- **malloc**(9), **free**(9), **calloc**(9), **realloc**(9) - kernel heap allocation
- **vmm_reserve**(9) - reserve large kernel virtual memory regions
- **vmm_hold**(9) - reserve virtual address space without mapping
- **pm_incref**(9), **pm_decref**(9), **pm_refcount**(9) - physical page reference counting

### VFS (Virtual Filesystem)
- **resolve_path**(9) - resolve filesystem path to dentry
- **attach_vnode**(9) - attach vnode to dentry
- **new_vnode**(9) - create new vnode (inode)

### Kernel I/O
- **printf**(9) - kernel console output
- **sprintf**(9), **snprintf**(9) - string formatting

## Complete Alphabetical List

```
attach_vnode(9)      calloc(9)         free(9)           kthread_create(9)
make_proc_file(9)    malloc(9)         new_proc_vnode(9) new_vnode(9)
pm_decref(9)         pm_incref(9)      pm_refcount(9)    printf(9)
proc_sprintf(9)      realloc(9)        resolve_path(9)   sleep_thread(9)
snprintf(9)          sprintf(9)        syscall_register(9) vmm_hold(9)
vmm_reserve(9)
```

## Audience

These pages are intended for:
- Kernel module developers
- Kernel developers
- System programmers working on Nightingale internals

## Key Differences from Userspace APIs

While some functions share names with userspace functions (malloc, printf, etc.),
the kernel versions:

- Allocate from kernel heap instead of process heap
- Write to kernel console instead of stdout
- Run with full kernel privileges
- Cannot be interrupted by signals
- Must handle synchronization manually
- Can be called from interrupt handlers (with restrictions)

## Module Development

For writing kernel modules, the most commonly used APIs are:

1. **Module initialization**
   - Return `MODINIT_SUCCESS` or `MODINIT_FAILURE`

2. **Extending kernel functionality**
   - `make_proc_file(9)` - Add /proc entries
   - `syscall_register(9)` - Add system calls
   - `resolve_path(9)` + `attach_vnode(9)` - Create device files

3. **Background processing**
   - `kthread_create(9)` - Create kernel threads
   - `sleep_thread(9)` - Sleep threads

4. **Memory management**
   - `malloc(9)`, `free(9)` - Dynamic allocation
   - `vmm_reserve(9)` - Large allocations

5. **Debugging**
   - `printf(9)` - Kernel logging

## Safety and Best Practices

Kernel code must:
- Always validate userspace pointers before dereferencing
- Check return values for errors
- Handle NULL pointers gracefully
- Avoid blocking in interrupt handlers
- Use appropriate synchronization primitives
- Free all allocated resources
- Never trust userspace input

## Related Documentation

- `/doc/modules/` - Kernel module examples and documentation
- `/doc/modules/MODULE_API.md` - Higher-level module API guide
- `/doc/man2/` - System call interface (kernel-to-userspace)
- `/kernel/include/ng/` - Kernel header files

## Source Code

Kernel internal implementations:
- `/kernel/` - Core kernel code
- `/arch/x86_64/` - Architecture-specific code
- `/kernel/include/ng/` - Kernel internal headers

## Notes

- Some APIs may have limitations or simplified implementations
- Limitations are documented in the NOTES section of each page
- The kernel API is subject to change as Nightingale evolves
- Not all kernel functions are documented yet (work in progress)

## Contributing Documentation

When adding new kernel APIs, please document them with man9 pages following
the existing format. Include:
- Synopsis with function prototype
- Detailed description
- Parameter explanations
- Return values
- Context (where can it be called from)
- Examples
- Notes and limitations
- Related functions
