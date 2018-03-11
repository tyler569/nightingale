
target remote localhost:1234
symbol-file ./ngk
set architecture i386:x86-64
break kernel_main
continue
