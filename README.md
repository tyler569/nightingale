# Nightingale

[![Travis build](https://travis-ci.org/tyler569/nightingale.svg?branch=master)](https://travis-ci.org/tyler569/nightingale)

### Download: [ISO](http://nightingale.philbrick.dev/latest/ngos64.iso)
(Run with `./run.py -f ngos64.iso`)

## About

Nightingale is an operating system for x86\_64 and i686 I have been developing for over 3 years now to learn about low-level programming and operating system design.

Nightingale implements a mostly POSIX-like userland, though compliance is not a goal. I see POSIX as useful as a well-understood and documented interface, and one that a lot of existing software is written against.

![Screenshot](/prompt.png?raw=true)

For more specific feature and capability information, see [ABOUT.md](/ABOUT.md).

## Project map

- `kernel/` : the core of the operating system, implements memory management, process and threads, etc.
- `user/` : a reference userspace for the nightingale kernel, mostly oriented around exersizing kernel features and testing.
- `libc/` : nc, nightingale's libc implementation.
- `sh/` : the shell, the command line interface to the system.
- `modules/` : various example kernel modules.
- `linker/` : the module loader and dynamic linker.
- `man/` : manual pages, though only a few are written.
- `exp/` : experiements and future directions.
- `toolchain/` : gcc and binutils patches, and a script to build nightingale-gcc.
- `external/` : code I did not write, currently libm and the lua interpreter.
- `ci/` : cloud-build code, build dockerfiles for Travis.
- `tools/` : useful tools and helpful utilities.
- `notes/` : random thoughts and ideas.

## Building nightingale

- In the `toolchain/` directory, inspect `build-toolchain.bash` to see the dependancies and edit the constants to match your environment (install directory, parallelism, etc)
- Run `bash build-toolchain.bash` and ensure the resulting binaries are available on your PATH
    - `x86_64-nightingale-gcc --version` should show the GCC version if everything worked correctly.
- Execute `make` in the root of the project.
- To run, use `./run.py` - its help text will show the available options.

## Why 'nightingale'

I have a long history of naming my programming projects after birds, and also used to name all my servers after birds, other than that I just like the name, there isn't any deep symbolism or anything.

## Acknowledgements

I used many resources to learn what I needed to get to where I am, but a special shoutout goes to the [OSDev Wiki](https://wiki.osdev.org/Expanded_Main_Page) community, for their extensive and comprehensive reference material that I have made extensive use of.

I've used code from several other people, lots of credit goes to [lua](https://www.lua.org/), [the Sortix project](https://sortix.org/) for their libm, Nicholas J. Kain for the original implementation of my setjmp, and Krzysztof Gabis for writing the brainf\*\*k interpreter that was the first third-party software I ever ported to nightingale.

