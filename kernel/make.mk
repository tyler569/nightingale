
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

$(OUT): $(OBJ)
	$(MKDIR)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) -lk -llinker -lgcc

