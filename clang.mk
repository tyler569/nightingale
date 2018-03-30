# vim: set noet ts=8 sw=8 sts=8

CC			= clang-5.0 -target x86_64-unknown-none
AS			= nasm -felf64
LD			= ld.lld

override CFLAGS	:= $(INCLUDE) -Wall -std=c11 -fno-lto -fpic		\
			-nostdlib -ffreestanding -mcmodel=kernel      		\
			-mno-mmx -mno-sse -mno-sse2 -mno-3dnow   			\
			-mno-red-zone -fno-asynchronous-unwind-tables       \
			-fstack-protector-strong -fno-omit-frame-pointer    \
			-D__is_ng_kernel

