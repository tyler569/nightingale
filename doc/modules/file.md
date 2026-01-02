# file.ko - Character Device Module

## Description

Demonstrates how a kernel module can create character device files in `/dev`.
This module creates `/dev/modfile` that generates sequential byte patterns
when read.

## Purpose

- Shows how to create device files from modules
- Demonstrates file operations implementation
- Example of character device driver

## Functionality

On load, the module:
1. Prints "Hello World from this kernel module!"
2. Creates a character device at `/dev/modfile`
3. Registers custom read operation
4. Sets file permissions to 0444 (read-only)
5. Returns `MODINIT_SUCCESS` on success

When `/dev/modfile` is read:
- Returns sequential bytes 0, 1, 2, 3, ... 255, 0, 1, ...
- Always fills the entire buffer with pattern
- Each read gets bytes based on buffer size

## Usage

```bash
# Load the module
insmod file.ko

# Read from the device
cat /dev/modfile | hexdump

# Or read specific amount
dd if=/dev/modfile bs=16 count=1 | hexdump -C
```

## Expected Output

Reading 16 bytes:
```
00000000  00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
```

## Source Code

**Location:** `/kernel/modules/file.c`

```c
#include <ng/fs.h>
#include <ng/mod.h>
#include <stdio.h>

ssize_t my_file_read(struct file *ofd, char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        ((char *)buf)[i] = (char)i;
    }
    return (ssize_t)len;
}

struct file_ops my_file_ops = {
    .read = my_file_read,
};

int make_my_file(const char *name) {
    struct dentry *path = resolve_path(name);
    if (IS_ERROR(path)) {
        printf("error %li creating %s\n", -ERROR(path), name);
        return MODINIT_FAILURE;
    }
    if (dentry_vnode(path)) {
        printf("error creating %s: already exists\n", name);
        return MODINIT_FAILURE;
    }

    struct vnode *vnode = new_vnode(path->file_system, 0444);
    vnode->type = FT_CHAR_DEV;
    vnode->file_ops = &my_file_ops;
    attach_vnode(path, vnode);
    return MODINIT_SUCCESS;
}

int init(struct mod *) {
    printf("Hello World from this kernel module!\n");
    return make_my_file("/dev/modfile");
}
```

## API Usage

**`resolve_path(path)`**
- Resolves a filesystem path to a dentry
- Returns dentry pointer or error

**`new_vnode(filesystem, mode)`**
- Creates a new vnode (inode) with specified permissions
- `mode` is octal permission bits (0444 = read-only)

**`attach_vnode(dentry, vnode)`**
- Attaches vnode to dentry, making file visible
- Creates the file in the filesystem

**`struct file_ops`**
- Defines file operations (read, write, ioctl, etc.)
- Only `read` is implemented in this example

## File Operations

The `read` operation:
- Receives file descriptor, buffer, and length
- Must fill buffer with data
- Returns number of bytes read
- Negative return indicates error

## Notes

- The device file persists for system lifetime
- No write operation implemented (read-only)
- No seek support (sequential reads only)
- Buffer is in kernel space (no copy_to_user needed)

## Use Cases

Character devices from modules are useful for:
- Hardware device drivers
- Virtual devices (random, null, zero, etc.)
- Debugging interfaces
- Test data generators

## See Also

- `/dev/` - Device file directory
- `/kernel/fs/char_dev.c` - Character device support
- `mknod(2)` - Create device files
- `read(2)` - Read from file descriptor
