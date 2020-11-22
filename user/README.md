## Nightingale usermode

Following is a listing of the programs and files included in the base
nightingale usermode and a brief description of each.

- `ab.c` : A minimal multiprocessing a/b test
- `args.c` : A program that indicates what arguments were passed to it
- `bf.c` : A brainf\*\*k interpreter
- `bg.c` : A program that spawns a background process that does nothing
- `bomb.c` : A fork bomb (for stress testing)
- `busy.c` : A program that runs a busy loop for a while
- `cat.c` : The standard POSIX `cat`
- `clone.c` : Test multithreading within a single process
- `crash.c` : Create different crash conditions (see `--help`)
- `create.c` : Intended to test the sys\_create syscall, this does not work
- `echo.c` : Echo arguments to stdout
- `echoserv.c` : A UDP echo server
- `false.c` : Do nothing, unsuccesfully
- `fcat.c` : A variant of `cat`, implemented with the `f` stdio functions
- `fib.lua` : A lua program for testing lua
- `fio.c` : A testing program for the `f` stdio functions
- `float.c` : For testing floating point math in multiple threads
- `forks.c` : For testing fork
- `hog.c` : Hogs the CPU (for stress testing)
- `init.c` : The init program launches by the kernel at boot
- `insmod.c` : Loads a kernel module
- `kill.c` : Sends a signal
- `ls.c` : List contents of a directory
- `malloctest.c` : Test the malloc impelemntation
- `multiread.c` : Read from the same file in multiple processes
- `net.c` : Networking testing
- `pipe.c` : Pipe testing
- `polltest.c` : sys\_poll testing
- `README.md` : This README
- `rot13.c` : Reads from stdin and writes rot13(input) to stdout - mainly for
  pipe testing
- `rsh.c` : UDP reverse shell
- `segv.c` : Performs a segfault (for signal testing)
- `sg.c` : Waits for SIGINT and runs a signal handler
- `sleep.c` : Sleep for some time
- `strace.c` : Set the `strace` flag on a process (causing it to print system
  calls in the kernel)
- `test.c` : Some basic validation testing
- `test.lua` : Another simple lua program for testing lua
- `text_file` : A text file
- `threads.c` : Test program that spawns many processes and cleans them up
- `time.c` : Time a subprocess runtime
- `top.c` : Prints the running processes and threads
- `udpnc.c` : A UDP connection tester
- `uname.c` : Prints the system uname
- `what.c` : Shows the current pid and thread id
- `write.c` : Writes to a test file
