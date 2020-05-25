DIR := libc
BDIR := crt
CSRC := 
ASRC := $(DIR)/crt0.S
COBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(CSRC))
AOBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(ASRC))
OBJ := $(COBJ) $(AOBJ)
OUT := $(SYSLIB)/crt0.o

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
