## Nightingale

An x86 Kernel and to-be OS written in C

`make` builds the kernel image

`make iso` generates an iso using grub at `nightingale.iso`

`make run` runs that iso in qemu

`make debug` runs qemu with `-s -S`, meaning it will wait for a connection from gdb on (by default) localhost:1234.
The .gdbinit file in this directory automatically configures gdb to connect to this qemu backend and load the kernel symbols when it is opened.

### TODO

- [ ] Audit arch/ to ensure it is everything x86-specific, and add a generic interface to include/arch.
- [X] Physical memory locator / page frame allocator (improve)
- [X] Expandable kernel heap (improve)
- [ ] Move stdlib things to libk (maybe)
- [X] Tasking and kernel threads (real processes - struct kthread or something)
  - [ ] Reorganize and improve this system.
  - [ ] include a vmm table
- [ ] Audit formatting and naming (perhaps in some automated way)
- [ ] Automated testing (\_\_human\_readable\_errors)

### What this project is

This OS is intended to help me learn more about x86, assembly, compiler internals, and operating system development.  As such, large contributions are discouraged, but bugfixes and suggestions are appreciated.

### Why 'nightingale'

This OS is as-yet unnamed, and I use the word 'nightingale' to refer to it.  I have a long history of naming my programming projects after birds, and also used to name all my servers after birds.  Nightingale is not the final name of this project.

### Acknowledgements

I used many resources to learn what I needed to get to where I am, but a special shoutout goes to the OSDev Wiki community, for their extensive and comprehensive reference material that I have made extensive use of.

