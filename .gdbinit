
target remote localhost:1234

symbol-file ./build-x86_64/ngk.elf
#symbol-file ./build-i686/ngk.elf

set architecture i386:x86-64
#set architecture i386

source gdb_utilities.py

break start_higher_half
#break start
break break_point

continue
