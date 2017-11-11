
set arch i386:x86-64
symbol-file ./nightingale.kernel

target remote localhost:1234

break main
continue
