
# ARCH_CFLAGS are already set when this file is included

export OPT=-Og
export DEBUG=-g
export LIBDIR=$(BUILDDIR)
export INCDIR=$(shell pwd)/include
export KERNELDIR=$(shell pwd)/kernel
WARNING=-Wall -Wextra -Wpedantic -pedantic -Werror

# kernel mode flags

export K_C_CXX_COMMON = \
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


export KCFLAGS	= $(OPT) $(DEBUG) $(ARCH_CFLAGS) $(WARNING) \
	$(K_C_CXX_COMMON) -std=c11

# user mode flags

export CFLAGS	= $(INCLUDE) $(OPT) $(DEBUG) $(WARNING) \
		  -std=c11 -nostdlib -ffreestanding \
		  -fno-omit-frame-pointer \
		  -Wno-unused-variable \
		  -Wno-unused-parameter \
		  -Wno-sign-compare \
		  -ffunction-sections \
		  -fno-strict-aliasing \
		  $(EXTRA_CFLAGS) $(EXTRA_WARNING)

export CXXFLAGS	= $(INCLUDE) $(OPT) $(DEBUG) $(WARNING) \
		  -xc++ -std=c++17 -nostdlib -ffreestanding \
		  -fno-omit-frame-pointer \
		  -Wno-unused-variable \
		  -Wno-unused-parameter \
		  -Wno-sign-compare \
		  -ffunction-sections \
		  -fno-strict-aliasing \
		  -fno-exceptions -fno-rtti \
		  $(EXTRA_CFLAGS) $(EXTRA_WARNING)

export CXX	= $(CC)

# kernel compilers (TODO)
export KCC	= $(CC)
export KCXX	= $(CXX)
# user compilers
export UCC	= $(CC)
export UCXX	= $(CXX)

export KASFLAGS =
export KLDFLAGS = -nostdlib -T$(LINKSCRIPT) -L$(LIBDIR) \
       		  -zmax-page-size=0x1000 $(DEBUG)

export LDFLAGS = -nostdlib $(DEBUG) -L$(LIBDIR)

export Q = @
export N = 2>/dev/null

define ECHO
@echo $(1) $(notdir $2)
endef
