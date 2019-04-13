
target remote localhost:1234
symbol-file ./buildX86_64/ngk
set architecture i386:x86-64
break start_higher_half
continue

