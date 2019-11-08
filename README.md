## Nightingale

A minimal operating system I'm writing to learn about osdev and low-level programming.

[![Travis build](https://travis-ci.org/tyler569/nightingale.svg?branch=master)](https://travis-ci.org/tyler569/nightingale)

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
- IPC through pipes and signals
- Basic networking (though work-in-progress)
    - Sending and receiving UDP datagrams from userspace
- Syscall interface
- Loadable kernel modules

Writing software for the nightingale is very similar to writing POSIX software today, several of the most important interfaces are supported.
This is intended to make porting existing software as easy as practicable, and make programming for the nightingale system familiar to programmers used to traditional POSIX systems.

As an illustrative example, this is the source of the `cat` program in the base distribution:
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

The same program compiles and runs unchanged on nightingale and on my Ubuntu Linux machine.

### Running nightingale

`make` builds the kernel image and an iso image at `ngos64.iso`

`./run.rb` runs that iso in qemu.  By default, it outputs the OS serial to the console.

`./run.rb -d` runs qemu with `-s -S`, meaning it will wait for a connection from gdb.
The provided .gdbinit file in this directory automatically configures gdb to connect to this qemu backend and load the kernel symbols when started with `gdb`.

The run script has a few other flags of note:
- `-v` Use the video output - switch from using serial to the VGA video, and use stdio for monitoring.
- `-i` Show interrupts (can be quite noisy with the timer enabled).
- `-m` Use stdio for qemu's monitor.
- `-d` Debug mode with gdb as described above.
- `-32`/`-64` Select 32 or 64 bit explictly.

More information can be found by running `./run.rb --help`

If you are interested in an ISO to run, the most recent build is available [here](http://nightingale.philbrick.dev/latest/ngos64.iso).
It can be run with the `./run.rb` script as described above, or with `qemu-system-x86_64 -cdrom ngos64.iso -vga std -no-reboot -m 256M -serial stdio -display none` if you prefer not to run ruby.

### Project map

- `kernel/` : the architecture-independant core (multitasking, memory management, ipc, etc.)
- `include/` : headers used externally (headers only used within one subsystem are with the source files)
- `user/` : a reference userspace for the nightingale kernel, mostly oriented around exersizing kernel features and testing
- `nc/` : nightingale's libc implementation
- `nx/` : nightingale's c++ library implementation (to do)
- `ci/` : cloud-build code, build dockerfiles for Travis
- `tools/` : useful tools and helpful utilities

### Why 'nightingale'

This OS is as-yet unnamed, and I use the word 'nightingale' to refer to it.  I have a long history of naming my programming projects after birds, and also used to name all my servers after birds.  Nightingale is not the final name of this project.

### Acknowledgements

I used many resources to learn what I needed to get to where I am, but a special shoutout goes to the OSDev Wiki community, for their extensive and comprehensive reference material that I have made extensive use of.

### TODO

- Improve networking
    - TCP
    - Proper routing engine
- Interprocess shared memory
- Improve filesystem support
    - More file types
    - Disk filesystem support
- Add device drivers for:
    - Disk controllers
    - Keyboard
    - Bitmapped graphics
    - More network cards
- More ports
    - ARM
    - RISC-V
    - Other ports?
- Improve loadable modules
- Multicore (SMP)
- Automated testing
- Add documentation that isn't comments
- Seperate the modules better
    - Make `nc` work on Linux
- Fix all the bugs

