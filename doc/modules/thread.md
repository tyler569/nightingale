# thread.ko - Kernel Thread Module

## Description

Demonstrates how a kernel module can create kernel threads that run in the
background. This module creates a thread that prints a message every second.

## Purpose

- Shows how to create kernel threads from modules
- Demonstrates long-running background tasks
- Example of periodic kernel work

## Functionality

On load, the module:
1. Prints "Hello World from this kernel module!"
2. Prints "Imma make a thread now!"
3. Creates a new kernel thread running `mod_kthread()`
4. Returns `MODINIT_SUCCESS`

The kernel thread:
1. Prints "This is the thread!" on startup
2. Enters an infinite loop
3. Sleeps for 1 second each iteration
4. Runs indefinitely

## Usage

```bash
insmod thread.ko
```

## Expected Output

On module load:
```
Hello World from this kernel module!
Imma make a thread now!
This is the thread!
```

Then every second, the thread runs (silently sleeping).

## Source Code

**Location:** `/kernel/modules/thread.c`

```c
#include <ng/mod.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <stdio.h>

void mod_kthread(void *) {
    printf("This is the thread!\n");
    while (true)
        sleep_thread(seconds(1));
}

int init_mod(struct mod *) {
    printf("Hello World from this kernel module!\n");
    printf("Imma make a thread now!\n");
    kthread_create(mod_kthread, nullptr);
    return MODINIT_SUCCESS;
}
```

## API Usage

**`kthread_create(fn, arg)`**
- Creates a new kernel thread
- `fn` is the thread function (void fn(void *))
- `arg` is passed to the thread function (nullptr here)
- Thread starts running immediately

**`sleep_thread(duration)`**
- Puts the calling thread to sleep
- `duration` is in kernel time units
- `seconds(n)` macro converts seconds to kernel time

## Notes

- The thread runs forever with no cleanup mechanism
- This demonstrates a potential resource leak (since modules can't be unloaded)
- In a production system, threads should have a shutdown mechanism
- The thread runs with kernel privileges

## Use Cases

Kernel threads from modules are useful for:
- Background processing
- Device polling
- Periodic maintenance tasks
- Asynchronous event handling

## See Also

- `/kernel/thread.c` - Thread implementation
- `clone0(2)` - Userspace thread creation
- `kthread_create()` - Kernel API for thread creation
