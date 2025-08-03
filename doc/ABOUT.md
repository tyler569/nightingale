# Features of nightingale

(Needs expansion)

## Nightingale boot information

```
vmm: protecting kernel code and rodata
     0xffffffff80109100 - 0xffffffff80119000
pic: initialized
mb: multiboot initailized
mmap:                0:     9fc00 type 1
mmap:            9fc00:       400 type 2
mmap:            f0000:     10000 type 2
mmap:           100000:   1ee0000 type 1
mmap:          1fe0000:     20000 type 2
mmap:         fffc0000:     40000 type 2
mmap: total usable memory: 33029120 (31MB + 511KB)
mb: kernel command line 'init=init'
mb: user init at 0xffffffff801ae000
initfs at 0xffffffff801ae000
pmm: using 0x7d4000 as the first physical page
vfs: filesystem initialized
/dev/serial: tty ready
/dev/serial2: tty ready
threads: process structures initialized
threads: finalizer thread running
threads: thread_timer started
pci: found  (8086:1237) at 00:00.0
pci: found  (8086:7000) at 00:01.0
pci: found  (8086:7010) at 00:01.1
pci: found  (8086:7113) at 00:01.3
pci: found  (1234:1111) at 00:02.0
pci: found  (8086:100e) at 00:03.0

********************************

The Nightingale Operating System
Version v0.10.3-16-g60c9e22

********************************

pit: actual divisor: 1193
timer: ticking at 1000 HZ
threads: usermode thread installed
initialization took: 37138269
cpu: allowing irqs
Hello World from a kernel thread
The message is 'get a cat'!
Welcome to Nightingale
Nightingale shell
$
```

## Debug info

```
$ crash -S
Thread: [8:8] ("crash") performed an access violation
Fault reading data:0x0 because page not present from kernel mode.
NULL pointer access?
Fault occurred at 0xffffffff8010f125
    rax:                0    r8 : ffffffffffffffff
    rbx:                0    r9 :                0
    rcx:                0    r10:                0
    rdx:                3    r11:                a
    rsp: ffffffffc175de40    r12:               31
    rbp: ffffffffc175de50    r13: ffffffffc06b4d50
    rsi:                0    r14: ffffffffc175df01
    rdi:                0    r15: ffffffffc175df48
    rip: ffffffff8010f125    rfl: [ P Z          ] (46)
    cr3:            9d000    pid: [8:8]
(0xffffffff8010f125) <sys_fault+0x55>
(0xffffffff801265da) <do_syscall+0x41a>
(0xffffffff8010b436) <c_interrupt_shim+0x536>
(0xffffffff8010c67e) <interrupt_shim+0x32>
(0x00000000002073db) <syscall1+0xb>
(0x00000000002071c0) <fault+0x10>
(0x0000000000201fb5) <main+0x75>
(0x000000000020751b) <nc_start+0xb>
(0x00000000002074ea) <_start+0x1a>
terminated by signal 14
$
```

## Shell features

The nightingale shell supports many advanced features, including pipelines and
file redirection in both directions

```
$ echo hello world
hello world
$ echo hello           world
hello world
$ echo "hello            world"
hello            world
$ echo hello world | rot13
uryyb jbeyq
$ echo hello world | rot13 > file
$ cat file
uryyb jbeyq
$ <file rot13
hello world
$
```

## Procfs

Nightingale exposes a dynamic filesystem with system information at `/proc`

```
$ ls /proc
th2%
test%
pmm%
timer%
th1%
1%
th3%
3%
th4%
4%
$ cat /proc/timer
The time is: 6714
Pending events:
  6715 (+1) "thread_timer"
$ cat /proc/1
1 '<init>' 0 0 0
$ cat /proc/th1
1 1 5 32 3 0 0 0 0 14 0 3741294358
```

The `%` is added by `ls` and indicates the files are procfiles -- the full list
of characters is:

```
directory:          '/'
character decice:   '^'
teletype:           '#'
socket:             ':'
pipe:               '&'
procfile:           '%'
```

## Syscall and event tracing

Nightingale supports two mechanisms for tracing user process events. `trace` is
a system call that is heavily-inspired by the Linux kernel's `ptrace` mechanism,
and exposes events like syscall entry and exists and signal deliveries to a
tracing thread. That thread has the ability to inspect and edit registers in the
traced thread.

```
$ trace echo "Hello World"
syscall_enter: execve
syscall_exit: execve -> 0
syscall_enter: mmap
syscall_exit: mmap -> 0
syscall_enter: write
Hello World syscall_exit: write -> 12
syscall_enter: write

syscall_exit: write -> 1
syscall_enter: exit
$
```

The other mechanism is known as "syscall trace" and is older, this is
implemented as a series of printf formats in the `syscall.c` file. When enabled,
the kernel itself will best-effort format syscalls and arguments.

```
$ strace echo "Hello World"
XX -> 1
[4:4] execve("echo", 0x00007fffff001008, (NULL)) -> 0
[4:4] mmap((NULL), 16777216, 3, 8, -1, 0) -> 0x100000000000
[4:4] write(1, 0x00007ffffefffcb0, 12)Hello World  -> 12
[4:4] write(1, 0x00007ffffeffff9f, 1)
 -> 1
[4:4] exit(0)$
$
```

This is currently more readable, but `trace` is still early in development.

## Loadable modules

The nightingale kernel supports loading modules at runtime that can extend
kernel functionality.

```
$ cat /proc/mod
open(): (ENOENT) Entity does not exist
-> 1
$ insmod procmod.ko
Loading procmod.ko
loaded mod "procfile module" to 0xffffffffc0064000, calling init
$ cat /proc/mod
Hello World from a kernel module
$
```

```
$ insmod thread.ko
Loading thread.ko
loaded mod "thread_mod" to 0xffffffffc0069000, calling init
Hello World from this kernel module!
Imma make a thread now!
This is the thread!
$
```

## Userspace environment

Writing software for the nightingale is very similar to writing POSIX software
today, several of the most important interfaces are supported. This is intended
to make porting existing software as easy as practicable, and make programming
for the nightingale system familiar to programmers used to traditional POSIX
systems.

```c
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
  char buf[128] = {0};

  for (char **arg = argv + 1; *arg; arg++) {
    int fd;
    if (strcmp(*arg, "-") == 0) {
      fd = STDIN_FILENO;
    } else {
      fd = open(*arg, O_RDONLY);
      if (fd < 0) {
        perror("open()");
        return EXIT_FAILURE;
      }
    }

    int count;

    while ((count = read(fd, buf, 128)) > 0) {
      write(STDOUT_FILENO, buf, count);
    }

    if (count < 0) {
      perror("read()");
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}
```

## Performance Flamegraphs
To use [flamegraph](http://www.brendangregg.com/flamegraphs.html) with nightingale,
uncomment `print_perf_trace` in `x86/interrupt.c`, ensure `serial2` is printing to a
file (`-serial file:./serial_perf` in `run.rb`), and run `flamegraph.bash`

