
# ARCH_CFLAGS are already set when this file is included

export OPT=-O0
export DEBUG=-g
export LIBDIR=$(BUILDDIR)
export INCDIR=$(shell pwd)/include
export KERNELDIR=$(shell pwd)/kernel
WARNING=-Wall -Wextra -Werror -Wpedantic -pedantic

# kernel mode flags

export KCFLAGS	= $(OPT) $(DEBUG) $(ARCH_CFLAGS) $(WARNING) \
		  -std=c11 -nostdlib -ffreestanding \
		  -mgeneral-regs-only \
		  -mno-red-zone -fno-asynchronous-unwind-tables \
		  -fno-strict-aliasing \
		  -fno-omit-frame-pointer \
		  -DNIGHTINGALE_VERSION="\"`git describe --tags`\"" \
		  -D__nightingale__=1 -D__kernel__=1 -D_NG=1 \
		  -Wno-unused-variable \
		  -Wno-unused-parameter \
		  -Wno-sign-compare \
		  -Wno-unused-function \
		  $(EXTRA_CFLAGS) $(EXTRA_WARNING)

export KCXXFLAGS= $(OPT) $(DEBUG) $(ARCH_CFLAGS) \
		  -Wall -Werror -Wextra \
		  -std=c++17 -nostdlib -ffreestanding \
		  -mgeneral-regs-only \
		  -mno-red-zone -fno-asynchronous-unwind-tables \
		  -fno-strict-aliasing \
		  -fno-omit-frame-pointer \
		  -fno-exceptions -fno-rtti \
		  -DNIGHTINGALE_VERSION="\"`git describe --tags`\"" \
		  -D__nightingale__=1 -D__kernel__=1 -D_NG=1 \
		  -Wno-unused-variable \
		  -Wno-unused-parameter \
		  -Wno-sign-compare \
		  -Wno-unused-function \
		  $(EXTRA_CFLAGS) $(EXTRA_WARNING)

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
