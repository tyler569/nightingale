
ifdef USE_CLANG
export CC=clang -target x86_64-unknown-none
else
export CC=x86_64-elf-gcc
endif
export AS=nasm -felf64
export LD=x86_64-elf-gcc

export BUILDDIR=$(shell pwd)/buildX86_64
export LINKSCRIPT=$(shell pwd)/kernel/x86/64/link_hh.ld

export ARCH_CFLAGS=-mcmodel=kernel

export ISO=$(shell pwd)/ngos64.iso

