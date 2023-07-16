# Nightingale

## About

Nightingale is an operating system for x86\_64 that I have been
developing for 6 years to learn about low-level programming and operating
system design.

Nightingale implements a mostly POSIX-like userland, though compliance is not a
goal. I see POSIX as useful as a well-understood and documented interface, and
one that permits compatability with large amounts of existing software.

![Screenshot](/doc/prompt.png?raw=true)

For more specific feature and capability information, see
[ABOUT.md](/doc/ABOUT.md).

## Project map

### Directories

- `doc`: Documentation
- `include`: Header files for the whole system
- `interface`: Interface definitions for syscalls and errno values
- `kernel`: The core of the operating system
- `libc`: Common userland routines, including things like `printf`
- `linker`: Kernel module loader, userland dynamic linker, and `libelf`
- `modules`: Kernel modules, mostly testing for now
- `script`: Utility scripts for building and developing nightingale
- `toolchain`: Patches and staging area for hosted gcc toolchain
- `user`: In-tree usermode programs distributed with the system

### Scripts

- `build_toolchain.bash`: builds the custom gcc for the
  `x86_64-nightingale` target
- `dump.rb`: convenience wrapper around `objdump`
- `flamegraph.bash`: convenience wrapper to call `flamegraph` - see
  [ABOUT.md](/doc/ABOUT.md)
- `format.bash`: convenience wrapper around the `clang-format` code formatter
- `generate_syscalls.rb`: render the syscall and errno manifests into C
  datastructures
- `install_headers.bash`: create `sysroot` and install system headers to the
  sysroot
- `run.rb`: convenience wrapper around `qemu-system-x86_64` to set the options I
  need

### Interface Manifests

These manifest files define the public syscall interface of the nightingale
kernel, they are rendered into C enums and metadata that is used by both the
kernel and the C library.

- `ERRNOS`: defines the values of `errno`, their names, and their `perror`
  strings
- `SYSCALLS`: defines syscall numbers, types, and arguments

## Building nightingale

Nightingale uses the CMake build system and defaults to the `clang` compiler.

The only uncommon package you will need is the `grub-mkrescue` tool provided by
grub2. This is usually packaged with grub2 or in a 'grub tools' package.

- Clang (default)
    - Execute `make` in the root of the project.
    - To run, use `./run.rb` - its help text will show the available options
      Clang (default)
- GCC (extra programs only)
    - Inspect `script/build_toolchain.bash` to see the dependencies and edit
      the constants to match your environment (install directory, parallelism,
      etc)
    - Run `script/build_toolchain.bash` and ensure the resulting binaries are
      available on your PATH
        - `x86_64-nightingale-gcc --version` should show the GCC version if
          everything worked correctly.
        - As written, the script assumes you have `~/.local/bin` on your PATH -
          you'll want to edit `PREFIX` and/or `PATH` to put it somewhere else.

I was able to build and run nightingale on a stock Ubuntu 20.04 image with the
following commands:
```bash
$ apt update
$ apt install git make cmake clang llvm lld ninja-build ruby xorriso mtools qemu-system
$ git clone https://github.com/tyler569/nightingale.git
$ make
$ ./run.rb
```

## Acknowledgements

I used many resources to learn what I needed to get to where I am, but a special
shout-out goes to the [OSDev Wiki](https://wiki.osdev.org/Expanded_Main_Page)
community, for their extensive and comprehensive reference material that I have
made extensive use of.

I've used code from several other people, lots of credit goes to
[lua](https://www.lua.org/), [the Sortix project](https://sortix.org/)
for their libm, Nicholas J. Kain for the original implementation of my setjmp,
and Krzysztof Gabis for writing the brainf\*\*k interpreter that was the first
third-party software I ever ported to nightingale.
