# Kernel Module API Reference

This document describes the API available to Nightingale kernel modules.

## Module Structure

### Required Module Info

Every module must define a `modinfo` structure:

```c
#include <ng/mod.h>

struct modinfo modinfo = {
    .name = "module_name",  // Module name (for identification)
    .init = modinit,        // Initialization function
};
```

### Initialization Function

```c
int modinit(struct mod *mod);
```

- Called when the module is loaded
- `mod` points to the module structure
- Return `MODINIT_SUCCESS` (0) on success
- Return `MODINIT_FAILURE` (-1) on failure
- Failure prevents module from loading

## Available APIs

### Proc Filesystem

**Create a proc file:**
```c
void make_proc_file(const char *name,
                    void (*callback)(struct file *, void *),
                    void *data);
```

- Creates `/proc/<name>`
- `callback` generates file content when read
- `data` is optional user data passed to callback

**Write to proc file:**
```c
void proc_sprintf(struct file *ofd, const char *format, ...);
```

- Formats output for proc files
- Works like `sprintf()` but for proc files

### System Calls

**Register a system call:**
```c
int syscall_register(int num,
                     const char *name,
                     sysret (*handler)(),
                     const char *signature,
                     int flags);
```

- Registers syscall at number `num`
- `name` is syscall name for debugging
- `handler` is the syscall implementation
- `signature` describes arguments (for tracing)
- `flags` controls behavior (0 for default)
- Returns syscall number on success

### Kernel Threads

**Create a kernel thread:**
```c
struct thread *kthread_create(void (*fn)(void *), void *arg);
```

- Creates new kernel thread running `fn(arg)`
- Thread starts immediately
- Returns thread structure pointer

**Sleep:**
```c
void sleep_thread(uint64_t duration);
```

- Sleeps current thread for `duration` time units
- Use `seconds(n)`, `milliseconds(n)` macros

### File System

**Resolve a path:**
```c
struct dentry *resolve_path(const char *path);
```

- Resolves path to dentry structure
- Returns dentry or error (check with `IS_ERROR()`)

**Create a vnode:**
```c
struct vnode *new_vnode(struct file_system *fs, mode_t mode);
```

- Creates new inode with permissions `mode`
- `fs` is the filesystem (from dentry)

**Attach vnode to dentry:**
```c
void attach_vnode(struct dentry *dentry, struct vnode *vnode);
```

- Links vnode to dentry, creating file

**File operations structure:**
```c
struct file_ops {
    ssize_t (*read)(struct file *, char *, size_t);
    ssize_t (*write)(struct file *, const char *, size_t);
    int (*ioctl)(struct file *, int, void *);
    // ... other operations
};
```

Set on vnode to implement custom file behavior.

## Standard Library Functions

Modules have access to kernel versions of standard functions:

### I/O
- `printf()` - Print to kernel console
- `sprintf()`, `snprintf()` - String formatting

### Memory
- `malloc()`, `free()` - Dynamic allocation
- `memcpy()`, `memset()`, `memmove()` - Memory operations

### String
- `strlen()`, `strcmp()`, `strncmp()` - String operations
- `strcpy()`, `strncpy()` - String copying

## Error Handling

**Check for errors:**
```c
if (IS_ERROR(ptr)) {
    int err = -ERROR(ptr);  // Extract error code
    printf("Error: %i\n", err);
    return MODINIT_FAILURE;
}
```

Error codes are defined in `/interface/ERRNOS`.

## Common Patterns

### Simple Hello World
```c
int modinit(struct mod *) {
    printf("Hello from module!\n");
    return MODINIT_SUCCESS;
}
```

### Create Proc File
```c
void my_proc(struct file *f, void *data) {
    proc_sprintf(f, "Content here\n");
}

int modinit(struct mod *) {
    make_proc_file("myfile", my_proc, NULL);
    return MODINIT_SUCCESS;
}
```

### Create Device File
```c
ssize_t my_read(struct file *f, char *buf, size_t len) {
    // Fill buffer
    return len;
}

struct file_ops my_ops = { .read = my_read };

int modinit(struct mod *) {
    struct dentry *d = resolve_path("/dev/mydev");
    if (IS_ERROR(d)) return MODINIT_FAILURE;

    struct vnode *v = new_vnode(d->file_system, 0444);
    v->type = FT_CHAR_DEV;
    v->file_ops = &my_ops;
    attach_vnode(d, v);
    return MODINIT_SUCCESS;
}
```

## Limitations

Current module system limitations:
- No module unloading
- No inter-module dependencies
- No parameter passing to modules
- No symbol versioning
- Global namespace for all symbols

## Best Practices

1. **Always validate inputs** - Check all pointers and values
2. **Handle errors gracefully** - Return `MODINIT_FAILURE` on errors
3. **Avoid infinite loops in init** - Keep initialization quick
4. **Be careful with resources** - No cleanup mechanism exists
5. **Test in VMs first** - Crashes can hang the system

## See Also

- `/kernel/modules/` - Example modules
- `/linker/modld.c` - Module loader implementation
- `/include/ng/mod.h` - Module header file
- `insmod(1)` - Module loading command
