Nightingale
===========

An x86 Kernel and to-be OS written in C

`make` builds the kernel image

`make iso` generates an iso using grub at `nightingale.iso`

`make run` runs that iso in qemu

`make debug` runs qemu with `-s -S`, meaning it will wait for a connection from gdb on (by default) localhost:1234.
The .gdbinit file in this directory automatically configures gdb to connect to this qemu backend and load the kernel symbols when it is opened.

Tasks
-----

- Resolve virtual memory -> physical address
- Allocate physical memory
- Map virtual memory
- Create a heap
- Create a Rust allocator crate and unlock heap-allocated types (Box, etc.)
- Use the framebuffer for something useful
- See what happens from there

What this project is
--------------------

This OS is intended to help me learn more about x86, assembly, compiler internals, and operating system development.  As such, large contributions are discouraged, but bugfixes and suggestions are appreciated.

What this project is not
------------------------

This OS is not intended to be usable in any real capacity - as an example, I do not intend to make it UNIX-like, and therefore will never be able to port existing programs to it.

Acknowledgements
----------------

I used many resources to learn what I needed to get to where I am, but special shoutouts go to the OSDev Wiki community.

