## Nightingale

An operating system.

### Features

Today, nightingale supports:

- x86\_64 and i686 ports
- Serial I/O for the boot console
- Discovering memory regions and PCI devices
- Arbitrary virtual memory management
- Multitasking, both in-kernel and seperate processes
- Userspace
    - Processes, loaded from ELF binaries
    - Executed in isolated memory maps
    - Allows for shared memory with threads
- Process management (wait)
- Basic networking (though work-in-progress)
    - Sending and receiving UDP datagrams from userspace
- Syscall interface with explicit "error" flag

Writing software for the nightingale is very similar to writing POSIX software today, several of the most important interfaces are supported.
This is intended to make porting existing software as easy as practicable, and make programming for the nightingale system familiar to programmers used to traditional POSIX systems.

As an illustrative example, this is the source of the `cat` program in the base distribution:
```c
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
  char buf[129] = {0};

  for (char **arg = argv + 1; *arg; arg++) {
    int fd = open(*arg, 0);
    if (fd < 0) {
      perror("open()");
      return EXIT_FAILURE;
    }

    int count;
    int total_count = 0;

    while ((count = read(fd, buf, 128)) > 0) {
      buf[count] = '\0';
      printf("%s", buf);
      total_count += count;
    }

    if (count < 0) {
      perror("read()");
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

```

The same program compiles and runs unchanged on nightingale and on my Ubuntu Linux machine.

### TODO

- Improve networking
    - TCP
    - Proper routing engine
- Signals and IPC
    - POSIX signals
    - Pipes
    - Shared memory
- Improve filesystem support
    - Directories
    - More file types
    - Disk filesystem support
- Add device drivers for:
    - Disk controllers
    - Keyboard
    - Bitmapped graphics
    - More network cards
- Add ARM port
    - Other ports?
- Loadable modules
- Multicore (SMP)
- Automated testing
- Fix all the bugs
- Documentation (in order of priority)
    - public interfaces (syscalls)
    - semi-public interfaces (things modules would use)
    - general working principles
    - private interfaces 

### Running nightingale

`make` builds the kernel image and an iso image at `ngos64.iso`

`./run.rb` runs that iso in qemu.  By default, it outputs the OS serial to the console.

`./run.rb -d` runs qemu with `-s -S`, meaning it will wait for a connection from gdb.
The provided .gdbinit file in this directory automatically configures gdb to connect to this qemu backend and load the kernel symbols when started with `gdb`.

The run script has a few other flags of note:
- `-v` Use the video output, switch from using serial to the VGA video, and use stdio for monitoring.
- `-i` Show interrupts (can be quite noisy with the timer enabled).
- `-m` Use stdio for qemu's monitor.
- `-d` Debug mode with gdb as described above.

More information can be found by running `./run.rb --help`

If you are interested in an ISO to run, the most recent build is available [here](http://nightingale.philbrick.dev/latest/ngos64.iso).
It can be run with the `./run.rb` script as described above, or with `qemu-system-x86_64 -cdrom ngos64.iso -vga std -no-reboot -m 256M -serial stdio -display none` if you prefer not to run ruby.

[![Travis build](https://travis-ci.org/tyler569/nightingale.svg?branch=master)](https://travis-ci.org/tyler569/nightingale)

### Project map

- `arch/` : architecture-specific code (mostly).  Support routines and initialization code
- `build_deps/` : cloud-build code, build dockerfiles for Travis
- `build$(ARCH)/` : build directory for objects
- `drv/` : device drivers
- `ds/` : datastructures with wide use across kernel systems
- `fs/` : filesystem code, both VFS/"files" and filesystem drivers
- `include/` : headers used externally (headers only used within one subsystem are with the source files)
- `kernel/` : the architecture-independant core (multitasking, memory management, ipc, etc.)
- `net/` : networking protocols, sockets, routing engine
- `tools/` : useful tools and helpful utilities
- `user/` : a reference userspace for the nightingale kernel, mostly oriented around exersizing kernel features and testing

### Why 'nightingale'

This OS is as-yet unnamed, and I use the word 'nightingale' to refer to it.  I have a long history of naming my programming projects after birds, and also used to name all my servers after birds.  Nightingale is not the final name of this project.

### Acknowledgements

I used many resources to learn what I needed to get to where I am, but a special shoutout goes to the OSDev Wiki community, for their extensive and comprehensive reference material that I have made extensive use of.

