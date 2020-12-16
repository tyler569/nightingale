# Nightingale

## About

Nightingale is an operating system for x86\_64 that I have been developing for
nearly 4 years to learn about low-level programming and operating system
design.

Nightingale implements a mostly POSIX-like userland, though compliance is not a
goal. I see POSIX as useful as a well-understood and documented interface, and
one that a lot of existing software is written against.

![Screenshot](/prompt.png?raw=true)

For more specific feature and capability information, see [ABOUT.md](/ABOUT.md).

## Project map

### Directories
- `ci`: cloud-build code, metadata for travis.ci
- `exp`: experiemental features and half-baked ideas
- `external`: code that I didn't write
- `fs`: filesystems and the virtual file system
- `include`: header files applicable to the whole project
- `kernel`: the core of the operating system. Implements memory management and threads
- `libc`: library routines, used by both the kernel and user programs
- `linker`: ELF library, module loader, and dynamic linker
- `man`: manual pages and documentation
- `modules`: kernel modules
- `sh`: the nightingale shell
- `scripts`: various utility scripts or helpers I wanted to save
- `sysroot`: compiler system root, this becomes `/` in the os's init filesystem
- `toolchain`: metadata to build custom hosted gcc
- `user`: experiemental userspace programs, oriented around testing the kernel
- `x86`: architecture-specific kernel code for the x86 platform

### Scripts
- `build_toolchain.bash`: builds the custom gcc for the `x86_64-nightingale` target
- `dump.rb`: convenience wrapper around `objdump`
- `flamegraph.bash`: convenience wrapper to call `flamegraph` - see [ABOUT.md](/ABOUT.md)
- `format.bash`: convenience wrapper around `clang-format`
- `generate_makefile.rb`: generate 
- `generate_syscalls.rb`: render the syscall and errno manifests into C datastructures
- `install_headers.bash`: create `sysroot` and install system headers to the sysroot
- `run.rb`: convenience wrapper around `qemu-system-x86_64` to set the options I need

### Manifests
These manifest files define the public syscall interface of the nightingale kernel,
they are rendered into C enums and metadata that is used by both the kernel and the
C library, and can also be used to create bindings for other languages.
- `ERRNOS`: defines the values of `errno`, their names, and their `perror` strings
- `SYSCALLS`: defines syscall numbers and types

## Building nightingale

- Inspect `build_toolchain.bash` to see the dependancies and edit the
  constants to match your environment (install directory, parallelism, etc)
- Run `build_toolchain.bash` and ensure the resulting binaries are
  available on your PATH
    - `x86_64-nightingale-gcc --version` should show the GCC version if
      everything worked correctly.
    - As written, the script assumes you have `~/.local/bin` on your PATH -
      you'll want to edit `PREFIX` and/or `PATH` to put it somewhere else.
- Execute `make` in the root of the project.
- To run, use `./run.rb` - its help text will show the available options.

## Why 'nightingale'

I have a long history of naming my programming projects after birds, and also
used to name all my servers after birds, other than that I just like the name,
there isn't any deep symbolism or anything.

## Acknowledgements

I used many resources to learn what I needed to get to where I am, but a special
shoutout goes to the [OSDev Wiki](https://wiki.osdev.org/Expanded_Main_Page)
community, for their extensive and comprehensive reference material that I have
made extensive use of.

I've used code from several other people, lots of credit goes
to [lua](https://www.lua.org/), [the Sortix project](https://sortix.org/) for
their libm, Nicholas J. Kain for the original implementation of my setjmp, and
Krzysztof Gabis for writing the brainf\*\*k interpreter that was the first
third-party software I ever ported to nightingale.

