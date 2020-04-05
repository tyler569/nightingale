
# ARCH_CFLAGS are already set when this file is included

export OPT=-Og
export DEBUG=-g
export LIBDIR=$(BUILDDIR)
export REPO=$(shell pwd)
export INCDIR=$(REPO)/include
export KERNELDIR=$(REPO)/kernel

export NIGHTINGALE=1

WARNING=-Wall -Wextra -Werror

# kernel mode flags

export KCFLAGS	= $(OPT) $(DEBUG) $(ARCH_CFLAGS) $(WARNING) \
		  -std=gnu18 \
		  -nostdlib \
		  -mno-red-zone \
		  -ffreestanding \
		  -fno-asynchronous-unwind-tables \
		  -fno-omit-frame-pointer \
		  -fno-strict-aliasing \
		  -DNIGHTINGALE_VERSION="\"`git describe --tags`\"" \
		  -D__nightingale__=1 -D__kernel__=1 -D_NG=1 \
		  -Wno-unused-variable \
		  -Wno-unused-parameter \
		  -Wno-sign-compare \
		  -Wno-unused-function \
		  -Wno-address-of-packed-member \
		  -Wno-array-bounds

# user mode flags

export CFLAGS	= $(INCLUDE) $(OPT) $(DEBUG) $(WARNING) \
		  -std=gnu18 -nostdlib -ffreestanding \
		  -fno-omit-frame-pointer \
		  -Wno-unused-variable \
		  -Wno-unused-parameter \
		  -Wno-sign-compare \
		  -ffunction-sections \
		  -fno-strict-aliasing \
		  -D__nightingale__=1 \
		  $(EXTRA_CFLAGS) $(EXTRA_WARNING)

# kernel compilers (TODO)
export KCC	= $(CC)
# user compilers
export UCC	= $(CC)

export KASFLAGS =
export KLDFLAGS = -nostdlib -T$(LINKSCRIPT) -L$(LIBDIR) \
       		  -zmax-page-size=0x1000 $(DEBUG)

export LDFLAGS = -nostdlib $(DEBUG) -L$(LIBDIR)

export Q = @
export N = 2>/dev/null

define ECHO
@echo $(1) $(notdir $2)
endef
