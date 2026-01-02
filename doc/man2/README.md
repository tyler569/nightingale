# Nightingale System Call Manual Pages

This directory contains manual pages for all Nightingale system calls.
The manual pages follow the standard Unix man page format (section 2).

## Viewing Manual Pages

You can view these pages with any man page viewer or text editor.
If you have `groff` installed, you can format them for terminal viewing:

```bash
groff -man -Tascii fork.2 | less
```

Or with `man` if your MANPATH includes this directory:

```bash
man 2 fork
```

## System Calls by Category

### Process Management
- **fork**(2) - create a new process
- **execve**(2), **execveat**(2) - execute program
- **_exit**(2), **exit_group**(2), **exit_thread**(2) - terminate process or thread
- **waitpid**(2) - wait for process to change state
- **getpid**(2), **gettid**(2) - get process and thread identification
- **clone0**(2) - create a new thread
- **create**(2) - create a new process from an executable
- **yield**(2) - yield the processor
- **setpgid**(2) - set process group ID
- **top**(2) - display process/thread information
- **procstate**(2) - query or modify process state
- **haltvm**(2) - halt the virtual machine

### Signal Handling
- **sigaction**(2) - examine and change a signal action
- **kill**(2) - send signal to a process
- **sigreturn**(2) - return from signal handler
- **sigprocmask**(2) - examine and change blocked signals

### File I/O
- **openat**(2) - open a file relative to a directory file descriptor
- **close**(2) - close a file descriptor
- **read**(2) - read from a file descriptor
- **write**(2) - write to a file descriptor
- **lseek**(2) - reposition read/write file offset
- **dup**(2), **dup2**(2) - duplicate a file descriptor
- **pipe**(2) - create pipe
- **ioctl**(2) - control device
- **getcwd**(2) - get current working directory
- **chdirat**(2) - change working directory
- **getdents**(2) - get directory entries
- **pathname**(2) - get pathname of a file descriptor
- **mkpipeat**(2) - make a named pipe (FIFO)

### File Metadata
- **fstat**(2), **statat**(2) - get file status
- **fchmod**(2), **chmodat**(2) - change permissions of a file
- **mkdirat**(2) - create a directory

### Filesystem Operations
- **linkat**(2) - make a new name for a file (hard link)
- **symlinkat**(2) - make a symbolic link
- **readlinkat**(2) - read value of a symbolic link
- **mknodat**(2) - create a special or ordinary file
- **unlinkat**(2) - delete a name from the filesystem
- **mountat**(2) - mount filesystem

### Memory Management
- **mmap**(2) - map files or devices into memory
- **munmap**(2) - unmap files or devices from memory

### System Information
- **uname**(2) - get system information
- **xtime**(2) - get time in milliseconds since boot
- **btime**(2) - get current calendar time
- **sleepms**(2) - sleep for specified milliseconds

### Debugging and Tracing
- **trace**(2) - process tracing and debugging
- **syscall_trace**(2) - enable or disable system call tracing
- **fault**(2) - trigger a specific fault for testing

### Special Purpose
- **loadmod**(2) - load a kernel module
- **settls**(2) - set thread-local storage base address
- **report_events**(2) - configure kernel event reporting
- **submit**(2) - submit asynchronous I/O operations

## Complete Alphabetical List

```
_exit(2)          exit_group(2)     exit_thread(2)    btime(2)
chdirat(2)        chmodat(2)        clone0(2)         close(2)
create(2)         dup(2)            dup2(2)           execve(2)
execveat(2)       fault(2)          fchmod(2)         fork(2)
fstat(2)          getcwd(2)         getdents(2)       getpid(2)
gettid(2)         haltvm(2)         ioctl(2)          kill(2)
linkat(2)         loadmod(2)        lseek(2)          mkdirat(2)
mknodat(2)        mkpipeat(2)       mmap(2)           mountat(2)
munmap(2)         openat(2)         pathname(2)       pipe(2)
procstate(2)      read(2)           readlinkat(2)     report_events(2)
setpgid(2)        settls(2)         sigaction(2)      sigprocmask(2)
sigreturn(2)      sleepms(2)        statat(2)         submit(2)
symlinkat(2)      syscall_trace(2)  top(2)            trace(2)
uname(2)          unlinkat(2)       waitpid(2)        write(2)
xtime(2)          yield(2)
```

## Notes

- These manual pages document the system call interface of Nightingale OS
- Some syscalls may have simplified or work-in-progress implementations
- Man pages marked with limitations are documented in their NOTES section
- For kernel implementation details, see the source code in `/kernel/`

## Related Documentation

- `/doc/ABOUT.md` - General information about Nightingale
- `/doc/DEBUGGING.md` - Debugging techniques
- `/CLAUDE.md` - Development guide for working with this codebase
- `/interface/SYSCALLS` - Syscall manifest used to generate headers
- `/interface/ERRNOS` - Error codes and messages
