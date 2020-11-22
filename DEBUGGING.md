## Debugging nightingale

Nightingale has several facilities to aid in debugging a problem. By default,
the system attempts to provide useful error messages for common error
conditions, such as unhandled pagfe faults, though this is certainly an area
that could be improved.

Nightingale can be stepped through in gdb, and if the run script is invoked
with `./run.rb -d`, it will will configure qemu to wait for a remote gdb
connection. The `.gdbinit` file in the project root then configures gdb to load
the correct symbols and sets an early initial break point so you have a chance
to set any watch or breakpoints you need. To start the system running from
inside gdb, use the command `continue`.

### Structure of a panic

Many panic conditions in nightingale will generate a register dump, stack trace,
and dump of the current stack. Here is an example of what this can look like:

```
Fault reading data:0x0 because page not present from kernel mode.
NULL pointer access?
Fault occured at 0xffffffff8010d8ab
    rax:                0    r8 :                0
    rbx: ffffffff80127daf    r9 :                0
    rcx:                1    r10: ffffffff80123078
    rdx:               10    r11:                0
    rsp: ffffffff850057f8    r12:               3c
    rbp: ffffffff85005ad8    r13:                0
    rsi:                0    r14:               11
    rdi: ffffffff8012ecc0    r15:                0
    rip: ffffffff8010d8ab    rfl: [ P Z  I       ] (246)
    cr3:           216000    pid:                2
backtrace from ffffffff85005ad8:
    bp: ffffffff85005ad8    ip: ffffffff801161f6
    bp: ffffffff85005b18    ip: ffffffff8011582e
    bp: ffffffff85005b58    ip: ffffffff80115f18
    bp: ffffffff85005b88    ip: ffffffff80123151
    bp: ffffffff85005dc8    ip: ffffffff801152ba
    bp: ffffffff85005e98    ip: ffffffff801183c8
    bp: ffffffff85005f08    ip: ffffffff80117f55
    bp: ffffffff85005f38    ip: ffffffff80106030
top of stack
Stack dump: (sp at 0xffffffff850057f8)
ffffffff850057b8: 0e00 0000 0000 0000 0000 0000 0000 0000   ................
ffffffff850057c8: abd8 1080 ffff ffff 0800 0000 0000 0000   ................
ffffffff850057d8: 4602 0000 0000 0000 f857 0085 ffff ffff   F........W......
ffffffff850057e8: 0000 0000 0000 0000 a7d8 1080 ffff ffff   ................
ffffffff850057f8: 0a00 0000 0000 0000 a07d 1280 ffff ffff   .........}......
ffffffff85005808: 0400 0000 0000 0000 0000 0000 ffff ffff   ................
ffffffff85005818: 0000 0000 0000 0000 0100 0000 20ff ffff   ............ ...
ffffffff85005828: 1000 0000 ffff ffff e85a 0085 ffff ffff   .........Z......
ffffffff85005838: 985a 0085 ffff ffff 7374 7275 6374 2076   .Z......struct v
[PANIC] 
```

You can see, the system first tries to provide an explanation of the faiure, in
this case that the running process attempted to access memory location 0 from
kernel mode. Next is the address where the fault originated and a dump of the
system registers, including the current process ID and virtual meory root.

Next is a stack trace expressed as pure memory addresses. You can use the dump
utilty (`./dump.rb`) to get an assembly dump of the kernel image and search for
these addresses, or alternatively, I provide a wrapper to `addr2line` in the
dump script by running `./dump.rb -a` that matches the instruction addresses to
the file and line where the call was made. The above example generates the
following trace:

```
*printf@print.c:444
print_vector@vector.c:66
vec_expand@vector.c:30
vec_push@vector.c:53
sys_socket@socket.c:189
do_syscall_with_table@syscall.c:153
syscall_handler@interrupt.c:229
c_interrupt_shim@interrupt.c:202
return_from_interrupt@??:?
```

This is enabled by saving the most recent output from the serial console in a
file (`last_output` by default) and then parsing the dump format. The `*` next
to printf indicates that it was the source of the fault.

