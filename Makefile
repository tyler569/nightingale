
TARGET		= x86_64-unknown-linux-gnu

KERNEL		= nightingale.kernel
LIB			= $(BUILDDIR)/libnightingale.a
ISO			= nightingale.iso

AS			= nasm -felf64
LD			= x86_64-elf-ld

QEMU		= qemu-system-x86_64
QEMUOPTS	= -cdrom $(ISO) -m $(MEM) -vga std -monitor stdio

SRCDIR		= kernel/src
BUILDDIR	= build

ASFLAGS		= -g
CARGOFLAGS  = --target=$(TARGET)
LDFLAGS		= -n -nostdlib -Tkernel/link.ld -z max-page-size=0x1000

ASOURCES	:= $(wildcard $(SRCDIR)/*.asm)
AOBJECTS	:= $(ASOURCES:$(SRCDIR)/%.asm=$(BUILDDIR)/%.o)
OBJECTS		:= $(AOBJECTS)

MEM			?= 64M

.PHONY:		all clean cleanall iso run debug dump

all: $(KERNEL)

$(KERNEL): $(OBJECTS) $(LIB)
	$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJECTS) $(LIB) --gc-sections

$(LIB): $(shell find $(SRCDIR) -type f)
	cd kernel; cargo build $(CARGOFLAGS)
	mkdir -p $(BUILDDIR)
	cp kernel/target/$(TARGET)/debug/libnightingale.a $(BUILDDIR)

$(BUILDDIR)/%.o : $(SRCDIR)/%.asm
	mkdir -p $(BUILDDIR)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f $(KERNEL)
	rm -f $(ISO)
	rm -rf $(BUILDDIR)
cleanall: clean
	rm -rf kernel/target

$(ISO): $(KERNEL)
	mkdir -p isodir/boot/grub
	cp kernel/grub.cfg isodir/boot/grub
	cp $(KERNEL) isodir/boot
	grub-mkrescue -o $(ISO) isodir/
	rm -rf isodir

iso: $(ISO)

run: $(ISO)
	$(QEMU) $(QEMUOPTS)

debug: $(ISO)
	$(QEMU) $(QEMUOPTS) -d cpu_reset -S -s

debugrst: $(ISO)
	$(QEMU) $(QEMUOPTS) -d cpu_reset

debugint: $(ISO)
	$(QEMU) $(QEMUOPTS) -d cpu_reset,int

dump: $(KERNEL)
	x86_64-elf-objdump -Mintel -d $(KERNEL) | less

