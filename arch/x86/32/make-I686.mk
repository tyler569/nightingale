
export CC=i686-elf-gcc
export AS=nasm -felf32
export LD=i686-elf-gcc

export BUILDDIR=$(shell pwd)/buildI686
export LINKSCRIPT=$(shell pwd)/arch/x86/32/link_hh.ld

export ARCH_CFLAGS=

export ISO=$(shell pwd)/ngos32.iso

