target remote localhost:1234

symbol-file ./build/kernel/nightingale_kernel
# symbol-file ./build/user/init

set architecture i386:x86-64
#set architecture i386

source gdb_utilities.py

break start_higher_half
# break start
break break_point

continue
