## Nightingale

An operating system.

`make` builds the kernel image and an iso image at `ngos64.iso`

`./run.rb` runs that iso in qemu.  By default, it outputs the OS serial to the console.

`./run.rb -d` runs qemu with `-s -S`, meaning it will wait for a connection from gdb.
The provided .gdbinit file in this directory automatically configures gdb to connect to this qemu backend and load the kernel symbols when started with `gdb`.

The run script has a few other flags of note:
- `-v` Use the video output, switch from using serial to the VGA video, and use stdio for monitoring.
- `-i` Show interrupts (can be quite noisy with the timer enabled).
- `-m` Use stdio for qemu's monitor.
- `-d` Debug mode with gdb as described above.

More information can be found by running `./run.rb --help`

If you are interested in an ISO to run, the most recent build is available [here](http://nightingale.philbrick.dev/latest/ngos64.iso).
It can be run with the `./run.rb` script as described above, or with `qemu-system-x86_64 -cdrom ngos64.iso -vga std -no-reboot -m 256M -serial stdio -display none` if you prefer not to run ruby.

### TODO

- [ ] Improve networking and rethink the architecture of how the kernel handles events
- [ ] Signals and IPC
- [ ] Pipes / FIFOs

- [ ] Add ARM port
- [X] Automated builds [![Travis build](https://travis-ci.org/tyler569/nightingale.svg?branch=master)](https://travis-ci.org/tyler569/nightingale)
- [ ] Automated testing

### What this project is

This OS is intended to help me learn more about x86, assembly, compiler internals, and operating system development.  As such, large contributions are discouraged, but bugfixes and suggestions are appreciated.

### Why 'nightingale'

This OS is as-yet unnamed, and I use the word 'nightingale' to refer to it.  I have a long history of naming my programming projects after birds, and also used to name all my servers after birds.  Nightingale is not the final name of this project.

### Acknowledgements

I used many resources to learn what I needed to get to where I am, but a special shoutout goes to the OSDev Wiki community, for their extensive and comprehensive reference material that I have made extensive use of.

