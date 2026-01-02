# Nightingale User Command Manual Pages

This directory contains manual pages for Nightingale userland commands (man section 1).
These are the programs available to users from the shell.

## Viewing Manual Pages

You can view these pages with any man page viewer or text editor.
If you have `groff` installed:

```bash
groff -man -Tascii ls.1 | less
```

Or with `man` if your MANPATH includes this directory:

```bash
man 1 ls
```

## Commands by Category

### File Management
- **cat**(1) - concatenate and display files
- **ls**(1) - list directory contents
- **tree**(1) - display directory tree structure
- **pwd**(1) - print working directory
- **chmod**(1) - change file permissions
- **rm**(1) - remove files
- **stat**(1) - display file status
- **head**(1) - output first part of files

### System Information
- **uname**(1) - print system information
- **date**(1) - print date and time
- **top**(1) - display process information

### Process Management
- **kill**(1) - send signal to process
- **sleep**(1) - delay for specified time

### Shell and Initialization
- **sh**(1) - command shell
- **init**(1) - system initialization process

### Debugging and Tracing
- **strace**(1) - trace system calls (kernel console output)
- **trace**(1) - ptrace-based system call tracer (stderr output)

### Kernel Management
- **insmod**(1) - load kernel module

### Utilities
- **echo**(1) - display text
- **clear**(1) - clear terminal screen
- **hexdump**(1) - display file in hexadecimal
- **time**(1) - time command execution

## Complete Alphabetical List

```
cat(1)        chmod(1)      clear(1)      date(1)
echo(1)       head(1)       hexdump(1)    init(1)
insmod(1)     kill(1)       ls(1)         pwd(1)
rm(1)         sh(1)         sleep(1)      stat(1)
strace(1)     time(1)       top(1)        trace(1)
tree(1)       uname(1)
```

## Shell Usage

Most of these commands are invoked from the Nightingale shell (**sh**).
The shell supports:

- Command execution
- I/O redirection (`>`, `<`, `2>`)
- Pipes (`|`)
- Background jobs (`&`)

See **sh**(1) for complete shell documentation.

## Implementation Notes

### Differences from Standard Unix Commands

Many Nightingale commands are simplified versions of their Unix counterparts:

- **ls** - No sorting options, limited formatting
- **echo** - No escape sequence support
- **sleep** - Takes milliseconds instead of seconds
- **chmod** - Octal modes only, no symbolic modes
- **rm** - Single file only, no recursive deletion
- **head** - Always 10 lines, no -n option
- **date** - No format strings, UTC only
- **uname** - No option flags
- **top** - Non-interactive, single snapshot only

### Tracing Tools

Nightingale provides two system call tracing tools:

1. **strace**(1) - Kernel-level tracing via `syscall_trace(2)` syscall
   - Output goes to kernel console (serial)
   - Simpler, less overhead
   - Can follow child processes with `-f`

2. **trace**(1) - Userspace tracer using `trace(2)` (ptrace)
   - Output goes to stderr
   - Shows syscall entry/exit and return values
   - Interactive feedback

## File Locations

User commands are installed to:
- `/bin/` - Essential user commands

Kernel modules are located in:
- `/lib/modules/` - Loadable kernel modules (.ko files)

## Related Documentation

- `/doc/man2/` - System call manual pages
- `/doc/man3/` - Library function manual pages
- `/doc/ABOUT.md` - General information about Nightingale
- `/CLAUDE.md` - Development guide
- `/user/` - Source code for user commands

## Notes

Some commands may have limitations or simplified implementations
compared to traditional Unix systems. These are documented in the
NOTES section of each manual page.

For the system calls these commands use, see the corresponding
man 2 pages in `/doc/man2/`.
