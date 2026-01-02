# procmod.ko - Proc Filesystem Module

## Description

Demonstrates how a kernel module can create entries in the `/proc` filesystem.
This module creates `/proc/mod` that outputs a simple message when read.

## Purpose

- Shows how to register procfs files from modules
- Demonstrates dynamic filesystem extension
- Provides example of read-only proc entries

## Functionality

On load, the module:
1. Registers a proc file named "mod" at `/proc/mod`
2. Sets up a callback function that generates the file's content
3. Returns `MODINIT_SUCCESS` on successful registration

When `/proc/mod` is read:
- Returns the string: "Hello World from a kernel module\n"

## Usage

```bash
# Load the module
insmod procmod.ko

# Read the proc file
cat /proc/mod
```

## Expected Output

Reading `/proc/mod`:
```
Hello World from a kernel module
```

## Source Code

**Location:** `/kernel/modules/procmod.c`

```c
#include <ng/fs.h>
#include <ng/mod.h>
#include <stdio.h>

void module_procfile(struct file *ofd, void *) {
    proc_sprintf(ofd, "Hello World from a kernel module\n");
}

int modinit(struct mod *) {
    make_proc_file("mod", module_procfile, nullptr);
    return MODINIT_SUCCESS;
}

struct modinfo modinfo = {
    .name = "procmod",
    .init = modinit,
};
```

## API Usage

**`make_proc_file(name, callback, data)`**
- Creates a new proc file at `/proc/<name>`
- `callback` is called when the file is read
- `data` is optional user data passed to callback

**`proc_sprintf(file, format, ...)`**
- Formats output for proc files
- Works like standard `sprintf()` but writes to proc file buffer

## Notes

- The proc file persists for the lifetime of the system (no cleanup on unload)
- Proc files are read-only in this example
- Multiple modules can create different proc files

## See Also

- `/proc` - Process information filesystem
- `/kernel/fs/proc.c` - Proc filesystem implementation
- `/kernel/fs/proc_files.c` - Proc file helpers
