
target remote localhost:1234

set arch i386:x86-64
symbol-file ./nightingale.kernel

break main
continue
