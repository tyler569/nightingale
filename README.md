PROJECT NIGHTINGALE
===================

An x86 Kernel and to-be OS

`make` builds the kernel and places it in `./bin`

`make run` runs the OS in qemu with -kernel

`make iso` generates an iso using grub at `nightingale.iso`

`make grub` runs that iso in qemu using -cdrom

Inspration and tutorials are currently cited in source files

Tasks
-----

- Write to the screen
- GDI and IDT
- Accept input without crashing

Long Term
---------

- malloc
- scheduler
- os things
