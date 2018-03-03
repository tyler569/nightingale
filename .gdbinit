
target remote localhost:1234
symbol-file ./nightingale.kernel
set architecture i386:x86-64
break kernel_main
continue
