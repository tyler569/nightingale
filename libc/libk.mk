DIR := libc
BDIR := libk
CSRC := \
	$(DIR)/ctype.c \
	$(DIR)/string.c \
	$(DIR)/stdio.c \
	$(DIR)/malloc.c \
	$(DIR)/errno.c \
	$(DIR)/signal.c
ASRC := \
	$(DIR)/setjmp.S
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
