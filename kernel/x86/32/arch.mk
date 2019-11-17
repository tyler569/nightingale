
ifdef USE_CLANG
export CC=clang -target i686-unknown-none
else
export CC=i686-elf-gcc
endif
export AS=nasm -felf32
export LD=i686-elf-gcc

export BUILDDIR=$(shell pwd)/buildI686
export LINKSCRIPT=$(shell pwd)/kernel/x86/32/link_hh.ld

export ARCH_CFLAGS=

export ISO=$(shell pwd)/ngos32.iso

