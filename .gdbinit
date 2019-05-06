
target remote localhost:1234

symbol-file ./buildX86_64/ngk
#symbol-file ./buildI686/ngk

set architecture i386:x86-64
#set architecture i386

break start_higher_half
#break start

continue

