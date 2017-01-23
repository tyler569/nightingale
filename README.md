PROJECT NIGHTINGALE
===================

An x86 Kernel and to-be OS

`make` builds the kernel and places it in `./bin`

`make run` runs the OS in qemu with -kernel

`make iso` generates an iso using grub at `nightingale.iso`

`make cdrom` runs that iso in qemu using -cdrom

Inspration and tutorials are currently cited in source files

Tasks
-----

- Write to the screen - done
- GDI and IDT - done
- Accept input without crashing
- More logical seperation in source files:
    - arch/x86
    - drivers
    - kernel (what is currently in kernel.c, I suppose)

Long Term
---------

- malloc
- scheduler
- os things
