PROJECT NIGHTINGALE
===================

An x86 Kernel and to-be OS

`make` builds the kernel image

`make iso` generates an iso using grub at `nightingale.iso`

`make run` runs that iso in qemu

`make debug` runs qemu with `-s -S`, meaning it will wait for a connection from gdb on (by default) localhost:1234.  The .gdbinit file in this directory automatically configures gdb to connect to this qemu backend and load the kernel symbols when it is opened.

Inspiration and tutorials are currently cited in source files - that which was taken from tutorials will be reimplemented by me at some point

Tasks
-----

- Write to the screen - done
- GDI and IDT - done
- Accept input without crashing - done
- memory allocation for kernel
- scheduler for kernel threads
- userspace

What this project is
--------------------

This OS is intended to help me learn more about x86, assembly, compiler internals, and operating system development.  As such, large contributions are discouraged, but bugfixes and suggestions are appreciated.

What this project is not
------------------------

This OS is not intended to be usable in any real capacity - as an example, I do not intend to make it UNIX-like, and therefore will never be able to port existing programs to it.
