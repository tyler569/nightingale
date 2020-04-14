
### A simple linker

This project implements a simple ELF dynamic linker.

It performs relocations on a relocateable ELF object (commonly, a '.o' file).

To see this in action, `mod.c` is compiled by the makefile to a relocateable object, which is linked my `link.c` into an object that can be loaded into the process memory image of `demo.c` and have its functions called.

To see the demo:

```
$ make
$ ./link
$ ./demo
```

There is a lot of things to be improved here, including:
- I shouldn't be trolling the section headers for the string table and symbol table repeatedly, these are common enough that I should probably store an info structure.
- This currently works for x86\_64 only, x86\_32 should be pretty easy, and other architectures have not been considered.
