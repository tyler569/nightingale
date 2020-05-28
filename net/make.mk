DIR := net
BDIR := $(DIR)
CSRC := $(shell find $(DIR) -type f -name "*.c")
ASRC :=
COBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(CSRC))
AOBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(ASRC))
OBJ := $(COBJ) $(AOBJ)
OUT := $(BUILD)/net.o

NET := $(OUT)

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
	$(CC) $(CFLAGS) -Wl,-r -o $@ $^
