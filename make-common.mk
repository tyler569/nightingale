
INCLUDE=-I$(shell pwd)/include
OPT=-O0
DEBUG=-g
WARNING=-Wall -Wextra -Werror -Wpedantic -pedantic
EXTRA_CFLAGS=
EXTRA_WARNING= 

export KCFLAGS	= $(INCLUDE) $(OPT) $(DEBUG) \
		  $(ARCH_CFLAGS) $(WARNING) \
		  -std=c11 -nostdlib -ffreestanding \
		  -mgeneral-regs-only \
		  -mno-red-zone -fno-asynchronous-unwind-tables \
		  -fno-strict-aliasing \
		  -fno-omit-frame-pointer \
		  -DNIGHTINGALE_VERSION="\"`git describe --tags`\"" \
		  -D__nightingale__=1 -D__kernel__=1 \
		  -Wno-unused-variable \
		  -Wno-unused-parameter \
		  -Wno-sign-compare \
		  -Wno-unused-function \
		  $(EXTRA_CFLAGS) $(EXTRA_WARNING)

export KASFLAGS =
export KLDFLAGS = -nostdlib -T$(LINKSCRIPT) -zmax-page-size=0x1000 -g

export Q = @
export N = 2>/dev/null

define ECHO
@echo $(1) $(notdir $2)
endef
