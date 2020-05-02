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
