
ARCH := x86-64
TRIPLE := x86_64-nightingale

DEBUG := -g
OPT := -Og

PROJECT_ROOT := ..

BUILDDIR := $(PROJECT_ROOT)/build-$(ARCH)
BUILDDIR += /kernel

SOURCEDIR := .

CC := $(TRIPLE)-gcc
CFLAGS := -ffreestanding -mcmodel=kernel -mno-red-zone $(OPT)

LD := $(TRIPLE)-ld
LDFLAGS := -nostdlib -T$(LINKSCRIPT) -L$(LIBDIR) \
	-zmax-page-size=0x1000 $(DEBUG) $(OPT)

AS := $(TRIPLE)-gcc
ASFLAGS := 

$(BUILDDIR)/ngk: $(BUILDDIR)/%.o
	
$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c


