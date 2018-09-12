## Nightingale

An x86-64 operating system.

`make` builds the kernel image and an iso image at `ngos.iso`

`./run.rb` runs that iso in qemu.  By default, it outputs the OS serial to the console.

`./run.rb -d` runs qemu with `-s -S`, meaning it will wait for a connection from gdb.
The provided .gdbinit file in this directory automatically configures gdb to connect to this qemu backend and load the kernel symbols when started with `gdb`.

The run script has a few other flags of note:
- `-v` Use the video output, switch from using serial to the VGA video, and use stdio for monitoring.
- `-i` Show interrupts (can be quite noisy with the timer enabled).
- `-m` Use stdio for qemu's monitor.
- `-d` Debug mode with gdb as described above.

More information can be found by running `./run.rb --help`

### TODO

- [ ] TCP networking stack and sockets
- [ ] Signals and IPC
- [ ] Pipes

- [ ] Move toward portability (x86-32, aarch64)
- [ ] Unify libc instead of having in and out-of-kernel implementations
- [ ] Audit formatting and naming (perhaps in some automated way)
- [ ] Automated testing

### What this project is

This OS is intended to help me learn more about x86, assembly, compiler internals, and operating system development.  As such, large contributions are discouraged, but bugfixes and suggestions are appreciated.

### Why 'nightingale'

This OS is as-yet unnamed, and I use the word 'nightingale' to refer to it.  I have a long history of naming my programming projects after birds, and also used to name all my servers after birds.  Nightingale is not the final name of this project.

### Acknowledgements

I used many resources to learn what I needed to get to where I am, but a special shoutout goes to the OSDev Wiki community, for their extensive and comprehensive reference material that I have made extensive use of.

