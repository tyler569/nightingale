DIR := libc
BDIR := $(DIR)-so
CSRC := $(shell find $(DIR) -type f -name '*.c')
ASRC := $(DIR)/setjmp.S
COBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(CSRC))
AOBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(ASRC))
OBJ := $(COBJ) $(AOBJ)
OUT := $(SYSLIB)/libc.so

LIBC_SO := $(OUT)

$(OUT): CFLAGS := $(UCFLAGS) -nostdlib -ffreestanding -fpic -shared
$(OUT): INCLUDE := $(UINCLUDE)
$(COBJ): $(BUILD)/$(BDIR)/%.c.o: $(DIR)/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<
$(AOBJ): $(BUILD)/$(BDIR)/%.S.o: $(DIR)/%.S
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OUT): $(OBJ)
	$(MKDIR)
	$(LD) $(CFLAGS) -o $@ -lm $^
