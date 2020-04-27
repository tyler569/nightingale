
MKDIR = @if [ ! -d $(@D) ] ; then mkdir -p $(@D) ; fi

export ARCH ?= x86_64
export TRIP := $(ARCH)-nightingale
export CC := $(TRIP)-gcc
export LD := $(TRIP)-gcc
export AS := $(TRIP)-gcc
export AR := ar
export NASM := nasm

STD := -std=gnu11
WARNING := -Wall -Wextra -Werror
DEBUG := -g
OPT := -Og

NGROOT := $(shell pwd)

UINCLUDE := #-I$(NGROOT)/include -I$(NGROOT)/include/nc
KINCLUDE := -I$(NGROOT)/include -I$(NGROOT)/kernel/include -I$(NGROOT)/linker/include

UCFLAGS := $(STD) $(DEBUG) $(OPT) \
	-Wno-builtin-declaration-mismatch

ULDFLAGS :=

KCFLAGS := $(STD) $(WARNING) $(DEBUG) $(OPT) \
	-ffreestanding -mno-red-zone -nostdlib \
	-fno-asynchronous-unwind-tables \
	-fno-omit-frame-pointer \
	-fno-strict-aliasing \
	-DNIGHTINGALE_VERSION="\"`git describe --tags`\"" \
	-D__kernel__=1 -D_NG=1 \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-sign-compare \
	-Wno-unused-function \
	-Wno-address-of-packed-member \
	-Wno-array-bounds

ifeq ($(ARCH),x86_64)
KLINKSCRIPT := kernel/x86/64/link_hh.ld
NASMFLAGS := -felf64
KCFLAGS += -mcmodel=kernel
else ifeq ($(ARCH),i686)
KLINKSCRIPT := kernel/x86/32/link_hh.ld
NASMFLAGS := -felf32
endif

BUILD := $(NGROOT)/build-$(ARCH)
export SYSROOT := $(NGROOT)/sysroot
export SYSUSR := $(SYSROOT)/usr
export SYSBIN := $(SYSROOT)/usr/bin
export SYSLIB := $(SYSROOT)/usr/lib
export SYSINC := $(SYSROOT)/usr/include

ifdef CI
UCFLAGS += --sysroot="$(SYSROOT)"
ULDFLAGS += --sysroot="$(SYSROOT)"
KCFLAGS += --sysroot="$(SYSROOT)"
endif

ISO := ngos.iso

KLDFLAGS := -nostdlib -T$(KLINKSCRIPT) -L$(BUILD) \
	-zmax-page-size=0x1000 $(DEBUG)

GRUBCFG := $(NGROOT)/kernel/grub.cfg

.PHONY: all clean all-ng make-sysroot
all: make-sysroot all-ng

clean:
	rm -rf build-* sysroot
	make -C external/libm clean
	make -C external/lua clean

make-sysroot:
	sh make-sysroot.sh

### Includes

# TODO TODO TODO TODO TODO make-sysroot ? cp -r ? something!

### LibC

DIR := libc
BDIR := $(DIR)
CSRC := $(shell find $(DIR) -type f -name '*.c')
ASRC := $(DIR)/setjmp.S
COBJ := $(patsubst %,$(BUILD)/%.o,$(CSRC))
AOBJ := $(patsubst %,$(BUILD)/%.o,$(ASRC))
OBJ := $(COBJ) $(AOBJ)
OUT := $(SYSLIB)/libc.a

LIBC := $(OUT)

$(OUT): CFLAGS := $(UCFLAGS) -nostdlib -ffreestanding
$(OUT): INCLUDE := $(UINCLUDE)
$(COBJ): $(BUILD)/$(BDIR)/%.c.o: $(DIR)/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<
$(AOBJ): $(BUILD)/$(BDIR)/%.S.o: $(DIR)/%.S
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OUT): $(OBJ)
	$(MKDIR)
	$(AR) rcs $@ $^

### LibK

DIR := libc
BDIR := libk
CSRC := \
	$(DIR)/ctype.c \
	$(DIR)/string.c \
	$(DIR)/stdio.c \
	$(DIR)/malloc.c \
	$(DIR)/errno.c
ASRC :=
COBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(CSRC))
AOBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(ASRC))
OBJ := $(COBJ) $(AOBJ)
OUT := $(BUILD)/libk.a

LIBK := $(OUT)

$(OUT): CFLAGS := $(KCFLAGS)
$(OUT): INCLUDE := $(KINCLUDE) -I$(NGROOT)/include/nc
$(COBJ): $(BUILD)/$(BDIR)/%.c.o: $(DIR)/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<
$(AOBJ): $(BUILD)/$(BDIR)/%.S.o: $(DIR)/%.S
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OUT): $(OBJ)
	$(MKDIR)
	$(AR) rcs $@ $^

### CRT

DIR := libc
BDIR := crt
CSRC := 
ASRC := \
	$(DIR)/crt0.S \
	$(DIR)/crti.S \
	$(DIR)/crtn.S
COBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(CSRC))
AOBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(ASRC))
OBJ := $(COBJ) $(AOBJ)
OUT := $(SYSLIB)/crt0.o $(SYSLIB)/crti.o $(SYSLIB)/crtn.o

CRT := $(OUT)

$(OUT): CFLAGS := $(UCFLAGS) -nostdlib -ffreestanding
$(OUT): INCLUDE := $(UINCLUDE)
$(COBJ): $(BUILD)/$(BDIR)/%.c.o: $(DIR)/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<
$(AOBJ): $(BUILD)/$(BDIR)/%.S.o: $(DIR)/%.S
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OUT): $(SYSLIB)/%.o: $(BUILD)/$(BDIR)/%.S.o
	$(MKDIR)
	cp $< $@

### User Programs

DIR := user
BDIR := $(DIR)
PROGRAMS := init echo uname what bf strace bomb cat threads false forks \
	top clone insmod fcat args pipe sg kill write segv echoserv sleep bg \
	fio float malloctest net rsh udpnc ls rot13 time multiread create \
	crash ab hog head threadsw column bf2
CSRC := $(patsubst %,$(DIR)/%.c,$(PROGRAMS))
ASRC :=
COBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(CSRC))
AOBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(ASRC))
OBJ := $(COBJ) $(AOBJ)
OUT := $(patsubst %,$(SYSBIN)/%,$(PROGRAMS))

PROGRAMS := $(OUT)

$(OUT): CFLAGS := $(UCFLAGS)
$(OUT): INCLUDE := $(UINCLUDE)
$(COBJ): $(BUILD)/$(BDIR)/%.c.o: $(DIR)/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<
$(AOBJ): $(BUILD)/$(BDIR)/%.S.o: $(DIR)/%.S
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OUT): $(SYSBIN)/%: $(BUILD)/$(BDIR)/%.c.o $(LIBC) $(CRT)
	$(MKDIR)
	$(CC) $(CFLAGS) -o $@ $<

### Sh

DIR := sh
BDIR := $(DIR)
CSRC := $(shell find $(DIR) -type f -name '*.c')
ASRC := 
COBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(CSRC))
AOBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(ASRC))
OBJ := $(COBJ) $(AOBJ)
OUT := $(SYSBIN)/sh

SH := $(OUT)

$(OUT): CFLAGS := $(UCFLAGS)
$(OUT): INCLUDE := $(UINCLUDE)
$(OUT): OBJ := $(OBJ)
$(COBJ): $(BUILD)/$(BDIR)/%.c.o: $(DIR)/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<
$(AOBJ): $(BUILD)/$(BDIR)/%.S.o: $(DIR)/%.S
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OUT): $(OBJ) $(LIBC) $(CRT)
	$(MKDIR)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

### LibM

DIR := external/libm
OUT := $(SYSLIB)/libm.a

LIBM := $(OUT)

$(OUT): CFLAGS := $(UCFLAGS)
$(OUT): DIR := $(DIR)
$(OUT): $(shell find $(DIR) -type f -name '*.[cS]')
	$(MAKE) -C $(DIR) CFLAGS="$(CFLAGS)"
	$(MAKE) -C $(DIR) CFLAGS="$(CFLAGS)" install

### Lua

DIR := external/lua
OUT := $(SYSBIN)/lua

LUA := $(OUT)

$(OUT): CFLAGS := $(UCFLAGS) -Wno-attributes
$(OUT): LDFLAGS := $(ULDFLAGS)
$(OUT): DIR := $(DIR)
$(OUT): $(shell find $(DIR) -type f -name '*.c') $(LIBC) $(LIBM)
	$(MAKE) -C $(DIR) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"
	$(MAKE) -C $(DIR) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" install

### KLinker

DIR := linker
BDIR := $(DIR)
CSRC := linker/elf.c
ASRC :=
COBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(CSRC))
AOBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(ASRC))
OBJ := $(COBJ) $(AOBJ)
OUT := $(BUILD)/liblinker.a

LINKER := $(OUT)

$(OUT): CFLAGS := $(KCFLAGS)
$(OUT): INCLUDE := $(KINCLUDE)
$(COBJ): $(BUILD)/$(BDIR)/%.c.o: $(DIR)/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<
$(AOBJ): $(BUILD)/$(BDIR)/%.S.o: $(DIR)/%.S
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OUT): $(OBJ)
	$(MKDIR)
	$(AR) rcs $@ $^

### Kernel Modules

DIR := modules
BDIR := $(DIR)
CSRC := $(shell find $(DIR) -type f -name '*.c')
ASRC :=
# COBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.ko,$(CSRC))
# AOBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.ko,$(ASRC))
# OBJ := $(COBJ) $(AOBJ)
OUT := $(patsubst $(DIR)/%.c,$(SYSBIN)/%.ko,$(CSRC))

MODULES := $(OUT)

$(OUT): CFLAGS := $(KCFLAGS)
$(OUT): INCLUDE := $(KINCLUDE)
$(OUT): $(SYSBIN)/%.ko: $(DIR)/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -g0 -o $@ -c $<

### Kernel

DIR := kernel
BDIR := $(DIR)
CSRC := $(shell find $(DIR) -type f -name '*.c' | grep -v x86)
ifeq ($(ARCH),x86_64)
CSRC += $(shell find $(DIR)/x86 -type f -name '*.c' | grep -v 32)
ASRC := $(shell find $(DIR)/x86 -type f -name '*.asm' | grep -v 32)
else ifeq ($(ARCH),i686)
CSRC += $(shell find $(DIR)/x86 -type f -name '*.c' | grep -v 64)
ASRC := $(shell find $(DIR)/x86 -type f -name '*.asm' | grep -v 64)
endif
COBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(CSRC))
AOBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(ASRC))
OBJ := $(COBJ) $(AOBJ)
OUT := $(BUILD)/ngk.elf

KERNEL := $(OUT)

$(OUT): CFLAGS := $(KCFLAGS)
$(OUT): LDFLAGS := $(KLDFLAGS)
$(OUT): INCLUDE := $(KINCLUDE)
$(OUT): OBJ := $(OBJ)
$(COBJ): $(BUILD)/$(BDIR)/%.c.o: $(DIR)/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<
$(AOBJ): $(BUILD)/$(BDIR)/%.asm.o: $(DIR)/%.asm
	$(MKDIR)
	$(NASM) $(NASMFLAGS) -o $@ $<

$(OUT): $(OBJ) $(LINKER) $(LIBK)
	$(MKDIR)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) -lk -llinker -lgcc

### ===

### Init

DIR := $(SYSBIN)
FILES := $(shell find $(DIR) -type f)
OUT := $(BUILD)/init.tar

INIT := $(OUT)

$(OUT): DIR := $(DIR)
$(OUT): $(PROGRAMS) $(MODULES) $(LUA) $(SH) $(MODULES)
	$(info tar init.tar)
	@cd $(DIR); tar cf $@ $(notdir $^)

### Raw init (no deps -just sysroot)

.PHONY: rawinit

DIR := $(SYSBIN)
FILES := $(shell find $(DIR) -type f)
OUT := $(BUILD)/init.tar

rawinit: DIR := $(DIR)
rawinit: OUT := $(OUT)
rawinit: FILES := $(FILES)
rawinit:
	$(info tar init.tar)
	@cd $(DIR); tar cf $(OUT) $(notdir $(FILES))

### ISO

OUT := $(ISO)

$(OUT): $(KERNEL) $(INIT) $(GRUBCFG)
	mkdir -p isodir/boot/grub
	cp $(GRUBCFG) isodir/boot/grub
	cp $(KERNEL) isodir/boot/ngk
	cp $(INIT) isodir/boot/initfs
	grub-mkrescue -o $(ISO) isodir/
	rm -rf isodir

all-ng: $(ISO)