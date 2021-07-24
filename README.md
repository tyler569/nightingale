# Nightingale

## About

Nightingale is an operating system for x86\_64 that I have been
developing for 4 years to learn about low-level programming and
operating system design.

Nightingale implements a mostly POSIX-like userland, though compliance
is not a goal. I see POSIX as useful as a well-understood and documented
interface, and one that a lot of existing software is written against.

![Screenshot](/doc/prompt.png?raw=true)

For more specific feature and capability information, see
[ABOUT.md](/doc/ABOUT.md).

## Project map

### Directories
- TODO update for refactor

### Scripts
- `build_toolchain.bash`: builds the custom gcc for the
  `x86_64-nightingale` target
- `dump.rb`: convenience wrapper around `objdump`
- `flamegraph.bash`: convenience wrapper to call `flamegraph` - see
  [ABOUT.md](/doc/ABOUT.md)
- `format.bash`: convenience wrapper around `clang-format`
- `generate_syscalls.rb`: render the syscall and errno manifests into C
  datastructures
- `install_headers.bash`: create `sysroot` and install system headers to
  the sysroot
- `run.rb`: convenience wrapper around `qemu-system-x86_64` to set the
  options I need

### Interface Manifests These manifest files define the public syscall
interface of the nightingale kernel, they are rendered into C enums and
metadata that is used by both the kernel and the C library, and can also
be used to create bindings for other languages.
- `ERRNOS`: defines the values of `errno`, their names, and their
  `perror` strings
- `SYSCALLS`: defines syscall numbers and types

## Building nightingale
- Clang (default)
    - Run `./build_iso.bash`
    - TODO clarify for refactor
- GCC
    - Inspect `scripts/build_toolchain.bash` to see the dependancies and
      edit the constants to match your environment (install directory,
      parallelism, etc)
    - Run `scripts/build_toolchain.bash` and ensure the resulting
      binaries are available on your PATH
        - `x86_64-nightingale-gcc --version` should show the GCC version
          if everything worked correctly.
        - As written, the script assumes you have `~/.local/bin` on your
          PATH - you'll want to edit `PREFIX` and/or `PATH` to put it
          somewhere else.
    - Execute `make` in the root of the project.
    - To run, use `./run.rb` - its help text will show the available
      options.

## Why 'nightingale' I have a long history of naming my programming
projects after birds, and also used to name all my servers after birds,
other than that I just like the name, there isn't any deep symbolism or
anything.

## Acknowledgements

I used many resources to learn what I needed to get to where I am, but a
special shoutout goes to the [OSDev
Wiki](https://wiki.osdev.org/Expanded_Main_Page) community, for their
extensive and comprehensive reference material that I have made
extensive use of.

I've used code from several other people, lots of credit goes to
[lua](https://www.lua.org/), [the Sortix project](https://sortix.org/)
for their libm, Nicholas J. Kain for the original implementation of my
    setjmp, and Krzysztof Gabis for writing the brainf\*\*k interpreter
    that was the first third-party software I ever ported to
    nightingale.

